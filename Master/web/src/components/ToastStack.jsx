export function ToastStack({ toasts, onDismiss }) {
  return (
    <div class="toast-stack">
      {toasts.map((toast) => (
        <div key={toast.id} class={`toast-card toast-${toast.type || 'info'}`}>
          <div>
            <strong>{toast.title || 'Notification'}</strong>
            <p>{toast.desc || ''}</p>
          </div>
          <button type="button" class="btn btn-sm btn-link" onClick={() => onDismiss(toast.id)}>
            <i class="bi bi-x-lg"></i>
          </button>
        </div>
      ))}
    </div>
  )
}
