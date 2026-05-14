import { useState } from 'preact/hooks'
import { LiveCharts } from '../components/LiveCharts'
import { SectionCard } from '../components/SectionCard'
import { StatusPill } from '../components/StatusPill'
import { TerminalConsole } from '../components/TerminalConsole'
import { ToastStack } from '../components/ToastStack'
import { useDashboardState } from '../hooks/useDashboardState'

const MODES = [
  { id: 0, name: 'Manuel', description: 'Pilotage direct de la vanne' },
  { id: 1, name: 'Basic', description: 'Maintien de niveau avec plage cible' },
  { id: 2, name: 'PID', description: 'Boucle de regulation fine' }
]

const TIMING_BUDGETS = [15, 20, 33, 50, 100, 200, 500]
const DISTANCE_MODES = [
  { id: 1, name: 'Short' },
  { id: 2, name: 'Long' }
]
const CPU_FREQS = [240, 160, 80, 40, 20, 10]

export function HomePage() {
  const {
    data,
    logs,
    toasts,
    activeMode,
    isDisconnected,
    sendRaw,
    sendAction,
    sendParam,
    changeMode,
    setToasts
  } = useDashboardState()
  const [manualCommand, setManualCommand] = useState('')

  const turbineStatus = data.turbineStatus || data.Turbine.status || {}
  const etangStatus = data.etangStatus || data.Etang.status || {}
  const radiateurStatus = data.nodeTestStatus || data.Radiateur.status || {}

  const dismissToast = (id) => {
    setToasts((current) => current.filter((item) => item.id !== id))
  }

  return (
    <>
      <div class="dashboard-grid">
        <SectionCard title="Modes" subtitle="Strategie de regulation" icon="bi bi-sliders2-vertical">
          <div class="mode-list">
            {MODES.map((mode) => (
              <button
                key={mode.id}
                type="button"
                class={`mode-button ${activeMode === mode.id ? 'active' : ''}`}
                onClick={() => changeMode(mode.id)}
              >
                <strong>{mode.name}</strong>
                <span>{mode.description}</span>
              </button>
            ))}
          </div>
        </SectionCard>

        <SectionCard title="Energie" subtitle="Bilan courant" icon="bi bi-lightning-charge">
          <div class="metric-emphasis">{formatFixed(data.Energie, 2)} Wh</div>
          <div class="button-row compact">
            <button type="button" class="btn btn-outline-primary" onClick={() => sendRaw('resetEnergieMeter')}>
              Reset compteur
            </button>
            <button type="button" class="btn btn-outline-warning" onClick={() => sendRaw('DeepSleepServer')}>
              DeepSleep server
            </button>
            <button type="button" class="btn btn-outline-secondary" onClick={() => sendRaw('SpectrumScan')}>
              Spectrum scan
            </button>
          </div>
        </SectionCard>
      </div>

      <div class="dashboard-grid dashboard-grid-3">
        <SectionCard
          title="Turbine"
          subtitle="Noeud puissance"
          icon="bi bi-fan"
          actions={<StatusPill active={Boolean(turbineStatus.Active)} label={turbineStatus.Active ? 'En ligne' : 'Hors ligne'} />}
        >
          <div class="stat-grid">
            <Metric label="Position" value={`${formatFixed(data.Turbine.positionVanne)} %`} />
            <Metric label="Cible" value={`${formatFixed(data.Turbine.PositionVanneTarget)} %`} />
            <Metric label="Tacky" value={`${formatFixed(data.Turbine.tacky)} rpm`} />
            <Metric label="Puissance" value={`${formatFixed(data.Turbine.power)} W`} />
            <Metric label="Tension" value={`${formatFixed(data.Turbine.tension, 2)} V`} />
            <Metric label="Intensite" value={`${formatFixed(data.Turbine.intensite, 2)} A`} />
          </div>
          <div class="meter-block">
            <label class="form-label">Commande vanne</label>
            <input
              type="range"
              class="form-range"
              min="0"
              max="100"
              value={data.Turbine.PositionVanneTarget || 0}
              onInput={(event) => sendRaw(`${event.target.id || 'PositionVanneTarget'} ${event.target.value}`)}
            />
          </div>
          <div class="button-row">
            <ActionButton label="Fermeture totale" onClick={() => sendAction('TURBINE', 'FT')} />
            <ActionButton label="Ouverture totale" onClick={() => sendAction('TURBINE', 'OT')} />
            <ActionButton label="Calibration ADS" onClick={() => sendAction('TURBINE', 'calibrate')} />
            <ActionButton label="SavePref" onClick={() => sendAction('TURBINE', 'SavePref')} />
            <ActionButton label="Reboot" kind="warning" onClick={() => sendAction('TURBINE', 'Reboot')} />
          </div>
          <div class="inline-fields">
            <NumberField label="AC" value={data.Turbine.AC} onCommit={(value) => sendAction('TURBINE', `AC=${value}`)} />
            <NumberField label="ZC" value={data.Turbine.ZC} onCommit={(value) => sendAction('TURBINE', `ZC=${value}`)} />
            <NumberField label="AV" value={data.Turbine.AV} onCommit={(value) => sendAction('TURBINE', `AV=${value}`)} />
            <NumberField label="ZV" value={data.Turbine.ZV} onCommit={(value) => sendAction('TURBINE', `ZV=${value}`)} />
          </div>
          <div class="button-row compact">
            {CPU_FREQS.map((freq) => (
              <button key={freq} type="button" class="chip-button" onClick={() => sendAction('TURBINE', `CpuFreq=${freq}`)}>
                {freq} MHz
              </button>
            ))}
          </div>
        </SectionCard>

        <SectionCard
          title="Etang"
          subtitle="Capteur niveau"
          icon="bi bi-moisture"
          actions={<StatusPill active={Boolean(etangStatus.Active)} label={etangStatus.Active ? 'Mesure OK' : 'Noeud absent'} />}
        >
          <div class="stat-grid">
            <Metric label="Niveau" value={`${formatFixed(data.Etang.niveauEtangP, 2)} %`} />
            <Metric label="Distance" value={`${formatFixed(data.Etang.niveauEtang)} mm`} />
            <Metric label="Full" value={`${formatFixed(data.Etang.niveauRempli)} mm`} />
            <Metric label="Empty" value={`${formatFixed(data.Etang.niveauVide)} mm`} />
            <Metric label="VL53" value={String(data.Etang.vl53Status ?? '-')} />
            <Metric label="ROI" value={String(data.Etang.RoiCenter ?? '-')} />
          </div>
          <div class="meter-block">
            <label class="form-label">Niveau plein</label>
            <input type="range" class="form-range" min="0" max="1000" value={data.Etang.niveauRempli || 0} onChange={(event) => sendAction('ETANG', `setNiveauFull=${event.target.value}`)} />
            <label class="form-label">Niveau vide</label>
            <input type="range" class="form-range" min="1500" max="3000" value={data.Etang.niveauVide || 0} onChange={(event) => sendAction('ETANG', `setNiveauEmpty=${event.target.value}`)} />
          </div>
          <div class="button-row">
            <ActionButton label="Set niveau full" onClick={() => sendAction('ETANG', 'setNiveauFull')} />
            <ActionButton label="Set niveau TP" onClick={() => sendAction('ETANG', 'setNiveauTP')} />
            <ActionButton label="Set niveau empty" onClick={() => sendAction('ETANG', 'setNiveauEmpty')} />
            <ActionButton label="Led VL53" onClick={() => sendAction('ETANG', 'LedVL53')} />
            <ActionButton label="Save Pref" onClick={() => sendAction('ETANG', 'savePref')} />
          </div>
          <div class="button-row compact">
            {TIMING_BUDGETS.map((value) => (
              <button key={value} type="button" class="chip-button" onClick={() => sendAction('ETANG', `TimingBudget=${value}`)}>
                {value} ms
              </button>
            ))}
          </div>
          <div class="button-row compact">
            {DISTANCE_MODES.map((mode) => (
              <button key={mode.id} type="button" class="chip-button" onClick={() => sendAction('ETANG', `DistanceMode=${mode.id}`)}>
                {mode.name}
              </button>
            ))}
          </div>
        </SectionCard>

        <SectionCard
          title="Radiateurs"
          subtitle="Node test"
          icon="bi bi-thermometer-half"
          actions={<StatusPill active={Boolean(radiateurStatus.Active)} label={radiateurStatus.Active ? 'En ligne' : 'Inactif'} />}
        >
          <div class="stat-grid">
            <Metric label="Temperature" value={`${formatFixed(data.Radiateur.temp, 1)} °C`} />
            <Metric label="Rad1" value={data.Radiateur.Rad1 ? 'ON' : 'OFF'} />
            <Metric label="Rad2" value={data.Radiateur.Rad2 ? 'ON' : 'OFF'} />
            <Metric label="RSSI" value={formatOptional(radiateurStatus.RSSI, 'dBm')} />
          </div>
          <div class="button-row">
            <ActionButton label="Blink" onClick={() => sendAction('NODETEST', 'blink')} />
            <ActionButton label="DeepSleep" onClick={() => sendAction('NODETEST', 'DEEPSLEEP')} />
            <ActionButton label="ToggleScreen" onClick={() => sendAction('NODETEST', 'ToggleScreen')} />
            <ActionButton label="ScanWifi" onClick={() => sendAction('NODETEST', 'ScanWifi')} />
          </div>
        </SectionCard>
      </div>

      <LiveCharts activeMode={activeMode} etang={data.Etang} turbine={data.Turbine} />

      <div class="dashboard-grid dashboard-grid-2">
        <SectionCard title="Configuration mode" subtitle="Parametres de regulation" icon="bi bi-diagram-3">
          <div class="inline-fields">
            <NumberField label="bc.target" onCommit={(value) => sendParam('bc.target', value)} />
            <NumberField label="bc.min" onCommit={(value) => sendParam('bc.min', value)} />
            <NumberField label="bc.max" onCommit={(value) => sendParam('bc.max', value)} />
            <NumberField label="pidc.kp" onCommit={(value) => sendParam('pidc.kp', value)} />
            <NumberField label="pidc.ki" onCommit={(value) => sendParam('pidc.ki', value)} />
            <NumberField label="pidc.kd" onCommit={(value) => sendParam('pidc.kd', value)} />
            <NumberField label="AlertNivMin" onCommit={(value) => sendParam('AlertNivMin', value)} />
            <NumberField label="AlertNivMax" onCommit={(value) => sendParam('AlertNivMax', value)} />
          </div>
          <div class="button-row">
            <ActionButton label="SavePref" onClick={() => sendRaw('SavePref')} />
            <ActionButton label="Test notif" onClick={() => sendRaw('testNotif')} />
            <ActionButton label="Test notif group" onClick={() => sendRaw('testNotifGroup')} />
            <ActionButton label="Led notif" onClick={() => sendRaw('LedNotif')} />
          </div>
        </SectionCard>

        <SectionCard title="Commande brute" subtitle="Acces direct websocket" icon="bi bi-command">
          <div class="manual-command-row">
            <input
              type="text"
              class="form-control"
              value={manualCommand}
              onInput={(event) => setManualCommand(event.target.value)}
              placeholder="Notification=true"
            />
            <button type="button" class="btn btn-primary" onClick={() => {
              sendRaw(manualCommand)
              setManualCommand('')
            }}>
              Envoyer
            </button>
          </div>
          <div class="button-row compact">
            <button type="button" class="chip-button" onClick={() => sendRaw('Notification=true')}>Notif ON</button>
            <button type="button" class="chip-button" onClick={() => sendRaw('Notification=false')}>Notif OFF</button>
            <button type="button" class="chip-button" onClick={() => sendRaw('NotificationGroup=true')}>Group ON</button>
            <button type="button" class="chip-button" onClick={() => sendRaw('NotificationGroup=false')}>Group OFF</button>
            <button type="button" class="chip-button" onClick={() => sendRaw('AlertNivActif=true')}>Alert ON</button>
          </div>
        </SectionCard>
      </div>

      <TerminalConsole logs={logs} onSend={sendRaw} />

      {isDisconnected ? (
        <div class="connection-modal">
          <div class="connection-card">
            <p class="hero-eyebrow">Websocket</p>
            <h3>Connexion perdue</h3>
            <p>Reconnexion en cours vers l'ESP32.</p>
            <div class="spinner-border text-light" role="status"></div>
          </div>
        </div>
      ) : null}

      <ToastStack toasts={toasts} onDismiss={dismissToast} />
    </>
  )
}

function Metric({ label, value }) {
  return (
    <div class="metric-tile">
      <span>{label}</span>
      <strong>{value}</strong>
    </div>
  )
}

function ActionButton({ label, onClick, kind = 'secondary' }) {
  return (
    <button type="button" class={`btn btn-outline-${kind}`} onClick={onClick}>
      {label}
    </button>
  )
}

function NumberField({ label, value = '', onCommit }) {
  return (
    <label class="field-stack">
      <span>{label}</span>
      <input
        type="number"
        class="form-control"
        defaultValue={value}
        onChange={(event) => onCommit(event.target.value)}
      />
    </label>
  )
}

function formatFixed(value, digits = 0) {
  return Number(value || 0).toFixed(digits)
}

function formatOptional(value, suffix = '') {
  if (value === undefined || value === null || value === '') {
    return '-'
  }

  return `${value} ${suffix}`.trim()
}
