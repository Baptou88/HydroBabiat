import { useEffect, useRef, useState } from 'preact/hooks'
import { useWebSocket } from './useWebSocket'

const DEFAULT_DATA = {
  Energie: 0,
  Etang: {
    niveauRempli: 0,
    niveauVide: 0,
    niveauEtangP: 0,
    niveauEtang: 0,
    RoiCenter: 0,
    distanceMode: 0,
    timingBudget: 0,
    vl53Status: -1,
    status: {}
  },
  Turbine: {
    positionVanne: 0,
    PositionVanneTarget: 0,
    tension: 0,
    tensionBatterie: 0,
    tacky: 0,
    intensite: 0,
    power: 0,
    motorState: 0,
    motorStateStr: '',
    AC: 0,
    ZC: 0,
    AV: 0,
    ZV: 0,
    status: {}
  },
  Radiateur: {
    temp: 0,
    Rad1: false,
    Rad2: false,
    status: {}
  },
  turbineStatus: {},
  etangStatus: {},
  nodeTestStatus: {}
}

export function useDashboardState() {
  const [data, setData] = useState(DEFAULT_DATA)
  const [activeMode, setActiveMode] = useState(0)
  const [isDisconnected, setIsDisconnected] = useState(false)
  const [logs, setLogs] = useState([])
  const [toasts, setToasts] = useState([])
  const toastIdRef = useRef(0)

  const pushLog = (type, text) => {
    setLogs((current) => [
      ...current.slice(-79),
      {
        id: `${Date.now()}-${Math.random()}`,
        type,
        text,
        timestamp: new Date().toLocaleTimeString()
      }
    ])
  }

  const pushToast = (toast) => {
    const id = toastIdRef.current++
    setToasts((current) => [...current, { ...toast, id }])
    window.setTimeout(() => {
      setToasts((current) => current.filter((item) => item.id !== id))
    }, 4000)
  }

  const { send } = useWebSocket(
    (message) => {
      if (message.data) {
        setData((previous) => ({
          ...previous,
          ...message.data
        }))
      }
      if (message.monitor) {
        pushLog('info', message.monitor)
      }
      if (message.toast) {
        pushToast(message.toast)
      }
    },
    () => {
      setIsDisconnected(false)
      pushLog('info', 'WebSocket connecte')
    },
    () => {
      setIsDisconnected(true)
      pushLog('error', 'Connexion websocket perdue')
    }
  )

  useEffect(() => {
    fetch('/dataEtang')
      .then((response) => response.ok ? response.json() : null)
      .then((payload) => {
        if (payload) {
          setData((current) => ({
            ...current,
            Etang: {
              ...current.Etang,
              ...payload
            }
          }))
        }
      })
      .catch(() => {})

    fetch('/dataTurbine')
      .then((response) => response.ok ? response.json() : null)
      .then((payload) => {
        if (payload) {
          setData((current) => ({
            ...current,
            Turbine: {
              ...current.Turbine,
              ...payload
            }
          }))
        }
      })
      .catch(() => {})

    const pollInterval = window.setInterval(() => {
      if (!isDisconnected) {
        return
      }

      fetch('/dataEtang')
        .then((response) => response.ok ? response.json() : null)
        .then((payload) => {
          if (payload) {
            setData((current) => ({
              ...current,
              Etang: {
                ...current.Etang,
                ...payload
              }
            }))
          }
        })
        .catch(() => {})

      fetch('/dataTurbine')
        .then((response) => response.ok ? response.json() : null)
        .then((payload) => {
          if (payload) {
            setData((current) => ({
              ...current,
              Turbine: {
                ...current.Turbine,
                ...payload
              }
            }))
          }
        })
        .catch(() => {})
    }, 1500)

    return () => {
      window.clearInterval(pollInterval)
    }
  }, [isDisconnected])

  const sendRaw = (command) => {
    if (!command) {
      return
    }

    pushLog('command', command)
    send(command)
  }

  const sendAction = (node, action) => {
    sendRaw(`Action:${node}:${action};`)
  }

  const sendParam = (name, value) => {
    sendRaw(`${name}=${value}`)
  }

  const changeMode = async (mode) => {
    const response = await fetch(`/mode?modeNum=${mode}`)
    if (response.ok) {
      setActiveMode(mode)
      pushToast({ title: 'Mode', desc: `Mode ${mode} active`, type: 'success' })
    }
  }

  return {
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
  }
}
