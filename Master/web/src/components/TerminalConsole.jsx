import { useState } from 'preact/hooks'
import { SectionCard } from './SectionCard'

export function TerminalConsole({ logs, onSend }) {
  const [command, setCommand] = useState('')

  const handleSubmit = (event) => {
    event.preventDefault()
    if (!command.trim()) {
      return
    }

    onSend(command.trim())
    setCommand('')
  }

  return (
    <SectionCard title="Terminal" subtitle="Websocket monitor" icon="bi bi-terminal">
      <div class="terminal-surface">
        {logs.length === 0 ? <p class="terminal-empty">Aucun evenement pour le moment.</p> : null}
        {logs.map((log) => (
          <p key={log.id} class={`terminal-line terminal-${log.type}`}>
            <span>[{log.timestamp}]</span>
            {log.text}
          </p>
        ))}
      </div>
      <form class="terminal-form" onSubmit={handleSubmit}>
        <input
          type="text"
          class="form-control"
          value={command}
          onInput={(event) => setCommand(event.target.value)}
          placeholder="Action:TURBINE:Reboot;"
        />
        <button type="submit" class="btn btn-primary">Envoyer</button>
      </form>
    </SectionCard>
  )
}
