import { ThemeToggle } from './ThemeToggle'

const NAV_ITEMS = [
  { href: '/', key: 'home', label: 'Pilotage', icon: 'bi bi-speedometer2' },
  { href: '/programmateur', key: 'programmateur', label: 'Programmateur', icon: 'bi bi-calendar-event' },
  { href: '/fileSystem', key: 'filesystem', label: 'Fichiers', icon: 'bi bi-folder2-open' },
  { href: '/spectrumScan', key: 'spectrum', label: 'Spectrum', icon: 'bi bi-broadcast-pin' }
]

export function AppShell({ pageKey, title, subtitle, theme, onThemeChange, onLogout, authAction, children }) {
  return (
    <div class="app-shell">
      <header class="app-topbar">
        <div class="app-brand-wrap">
          <a class="app-brand" href="/">
            <img src="/icons/hydro-elec-512.svg" alt="HydroBabiat" width="40" height="40" />
            <span>
              HydroBabiat
              <small>Centre de pilotage</small>
            </span>
          </a>
          <nav class="app-nav">
            {NAV_ITEMS.map((item) => (
              <a
                key={item.key}
                class={`app-nav-link ${pageKey === item.key ? 'active' : ''}`}
                href={item.href}
              >
                <i class={`${item.icon} me-2`}></i>
                {item.label}
              </a>
            ))}
          </nav>
        </div>
        <div class="app-topbar-actions">
          <ThemeToggle theme={theme} onThemeChange={onThemeChange} />
          {authAction ? authAction : (
            <button type="button" class="btn btn-outline-light app-auth-btn" onClick={onLogout}>
              <i class="bi bi-box-arrow-right me-2"></i>
              Logout
            </button>
          )}
        </div>
      </header>

      <main class="app-main container-fluid">
        <section class="hero-panel">
          <p class="hero-eyebrow">ESP32 control surface</p>
          <div class="hero-row">
            <div>
              <h1>{title}</h1>
              <p>{subtitle}</p>
            </div>
          </div>
        </section>
        {children}
      </main>
    </div>
  )
}
