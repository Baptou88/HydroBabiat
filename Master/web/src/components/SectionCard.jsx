export function SectionCard({ title, subtitle, icon, actions, children, className = '' }) {
  return (
    <section class={`section-card ${className}`.trim()}>
      <header class="section-card-head">
        <div>
          <p class="section-kicker">{subtitle}</p>
          <h2 class="section-title">
            {icon ? <i class={`${icon} me-2`}></i> : null}
            {title}
          </h2>
        </div>
        {actions ? <div class="section-actions">{actions}</div> : null}
      </header>
      <div class="section-card-body">{children}</div>
    </section>
  )
}
