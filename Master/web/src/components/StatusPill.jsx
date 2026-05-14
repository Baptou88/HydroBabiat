export function StatusPill({ active, label }) {
  return (
    <span class={`status-pill ${active ? 'status-pill-online' : 'status-pill-offline'}`}>
      <span class="status-pill-dot"></span>
      {label}
    </span>
  )
}
