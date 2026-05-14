export function Navbar({ activeMode, onModeChange, energie }) {
  const modes = [
    { num: 0, label: 'Mode 1' },
    { num: 1, label: 'Mode 2' },
    { num: 2, label: 'Mode 3' }
  ]

  return (
    <header class="border-bottom bg-light sticky-top">
      <nav class="navbar navbar-expand-lg">
        <div class="container-fluid">
          <a class="navbar-brand" href="/">
            <i class="bi bi-water"></i>
            <strong> HydroBabiat</strong>
          </a>
          <button 
            class="navbar-toggler" 
            type="button" 
            data-bs-toggle="collapse"
            data-bs-target="#navbarNav"
          >
            <span class="navbar-toggler-icon"></span>
          </button>
          <div class="collapse navbar-collapse" id="navbarNav">
            <ul class="navbar-nav ms-auto">
              {modes.map(mode => (
                <li class="nav-item" key={mode.num}>
                  <button
                    class={`nav-link ${activeMode === mode.num ? 'active' : ''}`}
                    onClick={() => onModeChange(mode.num)}
                  >
                    {mode.label}
                  </button>
                </li>
              ))}
              <li class="nav-item">
                <span class="nav-link">
                  <i class="bi bi-lightning-charge"></i> {energie.toFixed(2)} Wh
                </span>
              </li>
            </ul>
          </div>
        </div>
      </nav>
    </header>
  )
}
