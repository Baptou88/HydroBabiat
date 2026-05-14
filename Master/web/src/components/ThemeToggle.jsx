const THEMES = [
  { id: 'light', label: 'Clair', icon: 'bi bi-sun-fill' },
  { id: 'dark', label: 'Sombre', icon: 'bi bi-moon-stars-fill' },
  { id: 'auto', label: 'Auto', icon: 'bi bi-circle-half' }
]

export function ThemeToggle({ theme, onThemeChange }) {
  const activeTheme = THEMES.find((item) => item.id === theme) ?? THEMES[2]

  return (
    <div class="dropdown">
      <button
        class="btn btn-outline-light border-0 nav-theme-toggle dropdown-toggle"
        type="button"
        data-bs-toggle="dropdown"
        aria-expanded="false"
      >
        <i class={activeTheme.icon}></i>
        <span class="ms-2 d-none d-lg-inline">{activeTheme.label}</span>
      </button>
      <ul class="dropdown-menu dropdown-menu-end nav-theme-menu">
        {THEMES.map((item) => (
          <li key={item.id}>
            <button
              type="button"
              class={`dropdown-item d-flex align-items-center justify-content-between ${item.id === theme ? 'active' : ''}`}
              onClick={() => onThemeChange(item.id)}
            >
              <span>
                <i class={`${item.icon} me-2`}></i>
                {item.label}
              </span>
              {item.id === theme ? <i class="bi bi-check2"></i> : null}
            </button>
          </li>
        ))}
      </ul>
    </div>
  )
}
