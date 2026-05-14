import { useEffect, useState } from 'preact/hooks'
import { SectionCard } from '../components/SectionCard'

export function ProgrammateurPage() {
  const [tasks, setTasks] = useState([])
  const [status, setStatus] = useState('')

  const refresh = async () => {
    const response = await fetch('/programmateurJson')
    const payload = await response.json()
    setTasks((payload.tasks || []).map((task, index) => ({ id: index, ...task })))
  }

  useEffect(() => {
    refresh().catch(() => setStatus('Impossible de charger le programmateur'))
  }, [])

  const updateTask = (id, patch) => {
    setTasks((current) => current.map((task) => task.id === id ? { ...task, ...patch } : task))
  }

  const saveTask = async (task) => {
    const formData = new URLSearchParams()
    formData.set('id', task.id)
    formData.set('name', task.name)
    formData.set('time', normalizeTime(task.time))
    formData.set('targetVanne', task.targetVanne ?? 0)
    formData.set('deepsleep', task.deepSleep ?? 0)
    if (task.active) {
      formData.set('active', 'true')
    }
    if (task.execOnce) {
      formData.set('execOnce', 'true')
    }

    const response = await fetch('/updateprogrammateur', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: formData.toString()
    })

    setStatus(response.ok ? `Tache ${task.name} mise a jour` : 'Echec de mise a jour')
  }

  const addTask = async () => {
    await fetch('/programmateur/new')
    await refresh()
    setStatus('Nouvelle tache ajoutee')
  }

  const saveAll = async () => {
    await fetch('/programmateur/sauvegarder')
    setStatus('Programmateur sauvegarde')
  }

  return (
    <div class="dashboard-grid dashboard-grid-2">
      <SectionCard title="Programmateur" subtitle="Actions planifiees" icon="bi bi-calendar2-week" className="dashboard-span-2">
        <div class="button-row compact">
          <button type="button" class="btn btn-primary" onClick={addTask}>Nouvelle tache</button>
          <button type="button" class="btn btn-outline-success" onClick={saveAll}>Sauvegarder</button>
          <button type="button" class="btn btn-outline-secondary" onClick={refresh}>Rafraichir</button>
        </div>
        {status ? <p class="status-text">{status}</p> : null}
      </SectionCard>

      {tasks.map((task) => (
        <SectionCard key={task.id} title={task.name || `Tache ${task.id + 1}`} subtitle={`ID ${task.id}`} icon="bi bi-alarm">
          <div class="form-check form-switch mb-3">
            <input class="form-check-input" type="checkbox" checked={Boolean(task.active)} onChange={(event) => updateTask(task.id, { active: event.target.checked })} />
            <label class="form-check-label">Active</label>
          </div>
          <label class="field-stack">
            <span>Nom</span>
            <input class="form-control" value={task.name || ''} onInput={(event) => updateTask(task.id, { name: event.target.value })} />
          </label>
          <div class="inline-fields mt-3">
            <label class="field-stack">
              <span>Heure</span>
              <input type="time" class="form-control" value={normalizeTime(task.time)} onInput={(event) => updateTask(task.id, { time: event.target.value })} />
            </label>
            <label class="field-stack">
              <span>Target vanne</span>
              <input type="range" class="form-range" min="0" max="100" value={task.targetVanne || 0} onInput={(event) => updateTask(task.id, { targetVanne: Number(event.target.value) })} />
              <small>{task.targetVanne || 0} %</small>
            </label>
            <label class="field-stack">
              <span>DeepSleep (ms)</span>
              <input type="number" class="form-control" value={task.deepSleep || 0} onInput={(event) => updateTask(task.id, { deepSleep: Number(event.target.value) })} />
            </label>
          </div>
          <div class="form-check form-switch mt-3">
            <input class="form-check-input" type="checkbox" checked={Boolean(task.execOnce)} onChange={(event) => updateTask(task.id, { execOnce: event.target.checked })} />
            <label class="form-check-label">Execution unique</label>
          </div>
          <div class="button-row compact mt-3">
            <button type="button" class="btn btn-outline-primary" onClick={() => saveTask(task)}>Mettre a jour</button>
          </div>
        </SectionCard>
      ))}
    </div>
  )
}

function normalizeTime(value) {
  if (!value) {
    return '00:00'
  }

  const [hours = '0', minutes = '0'] = String(value).split(':')
  return `${hours.padStart(2, '0')}:${minutes.padStart(2, '0')}`
}
