export function NotAuthPage() {
  const login = () => {
    fetch('/login').then(() => {
      window.location.href = '/'
    })
  }

  return (
    <section class="auth-panel">
      <div class="auth-panel-card">
        <p class="hero-eyebrow">Acces protege</p>
        <h2>Vous n'etes pas connecte</h2>
        <p>L'interface de supervision requiert une authentification HTTP avant de piloter l'installation.</p>
        <button type="button" class="btn btn-primary btn-lg" onClick={login}>
          <i class="bi bi-box-arrow-in-right me-2"></i>
          Se connecter
        </button>
      </div>
    </section>
  )
}
