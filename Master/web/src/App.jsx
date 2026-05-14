import { AppShell } from './components/AppShell'
import { useTheme } from './hooks/useTheme'
import { FileSystemPage } from './pages/FileSystemPage'
import { HomePage } from './pages/HomePage'
import { NotAuthPage } from './pages/NotAuthPage'
import { ProgrammateurPage } from './pages/ProgrammateurPage'
import { SpectrumScanPage } from './pages/SpectrumScanPage'

const PAGE_META = {
  home: {
    title: 'Pilotage temps reel',
    subtitle: 'Vue unifiee turbine, etang, radiateurs, energie et terminal.'
  },
  filesystem: {
    title: 'Gestion du filesystem',
    subtitle: 'Inspection SPIFFS, envoi OTA et upload de fichiers directement depuis le navigateur.'
  },
  programmateur: {
    title: 'Programmateur',
    subtitle: 'Edition des taches planifiees depuis les endpoints JSON deja exposes par le firmware.'
  },
  notAuth: {
    title: 'Authentification requise',
    subtitle: 'Connexion HTTP necessaire avant d afficher les pages de supervision.'
  },
  spectrum: {
    title: 'Spectrum scan',
    subtitle: 'Visualisation heatmap des scans RF et outillage de test pour le developpement.'
  }
}

export default function App() {
  const { theme, setTheme } = useTheme()
  const pageKey = resolvePageKey()
  const meta = PAGE_META[pageKey] || PAGE_META.home

  const logout = () => {
    fetch('/logout').then(() => {
      window.location.href = '/'
    })
  }

  const authAction = pageKey === 'notAuth'
    ? (
        <button type="button" class="btn btn-primary app-auth-btn" onClick={() => {
          fetch('/login').then(() => {
            window.location.href = '/'
          })
        }}>
          <i class="bi bi-box-arrow-in-right me-2"></i>
          Login
        </button>
      )
    : null

  return (
    <AppShell
      pageKey={pageKey}
      title={meta.title}
      subtitle={meta.subtitle}
      theme={theme}
      onThemeChange={setTheme}
      onLogout={logout}
      authAction={authAction}
    >
      {pageKey === 'filesystem' ? <FileSystemPage /> : null}
      {pageKey === 'programmateur' ? <ProgrammateurPage /> : null}
      {pageKey === 'notAuth' ? <NotAuthPage /> : null}
      {pageKey === 'spectrum' ? <SpectrumScanPage /> : null}
      {pageKey === 'home' ? <HomePage /> : null}
    </AppShell>
  )
}

function resolvePageKey() {
  const explicitPage = document.body?.dataset?.page
  if (explicitPage) {
    return explicitPage
  }

  const pathname = window.location.pathname.toLowerCase()
  if (pathname.includes('filesystem')) {
    return 'filesystem'
  }
  if (pathname.includes('programmateur')) {
    return 'programmateur'
  }
  if (pathname.includes('notauth')) {
    return 'notAuth'
  }
  if (pathname.includes('spectrumscan')) {
    return 'spectrum'
  }

  return 'home'
}
