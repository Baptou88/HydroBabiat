import { useState } from 'preact/hooks'

export function TerminalPanel() {
  const [logs, setLogs] = useState([])
  const [input, setInput] = useState('')

  const addLog = (type, text) => {
    const dt = new Date()
    const timestamp = dt.toLocaleTimeString()
    setLogs(prev => [...prev.slice(-49), { type, text, timestamp }])
  }

  const handleSubmit = (e) => {
    e.preventDefault()
    if (!input.trim()) return
    
    addLog('command', input)
    // Envoyer au serveur via WebSocket
    setInput('')
  }

  return (
    <div class="card border-top-0 rounded-0">
      <div class="card-header border-top">
        <h6 class="mb-0">
          <i class="bi bi-terminal"></i> Terminal
        </h6>
      </div>
      <div class="card-body" style={{ height: '200px', overflowY: 'auto', fontFamily: 'monospace' }}>
        {logs.length === 0 ? (
          <p class="text-muted mb-0">Prêt...</p>
        ) : (
          logs.map((log, idx) => (
            <p key={idx} class={`mb-0 text-${log.type === 'error' ? 'danger' : log.type === 'info' ? 'info' : 'muted'}`}>
              <span class="text-secondary">[{log.timestamp}]</span> {log.text}
            </p>
          ))
        )}
      </div>
      <div class="card-footer border-top">
        <form onSubmit={handleSubmit} class="input-group input-group-sm">
          <span class="input-group-text">&gt;</span>
          <input
            type="text"
            class="form-control"
            placeholder="Entrez une commande..."
            value={input}
            onChange={(e) => setInput(e.target.value)}
          />
          <button type="submit" class="btn btn-sm btn-primary">
            Envoyer
          </button>
        </form>
      </div>
    </div>
  )
}
