import { useEffect, useRef, useCallback } from 'preact/hooks'

export function useWebSocket(onMessage, onOpen, onClose) {
  const wsRef = useRef(null)
  const reconnectTimeoutRef = useRef(null)
  const shouldReconnectRef = useRef(true)
  const onMessageRef = useRef(onMessage)
  const onOpenRef = useRef(onOpen)
  const onCloseRef = useRef(onClose)

  useEffect(() => {
    onMessageRef.current = onMessage
    onOpenRef.current = onOpen
    onCloseRef.current = onClose
  }, [onMessage, onOpen, onClose])

  const connect = useCallback(() => {
    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:'
    const wsUrl = `${protocol}//${window.location.host}/ws`

    const ws = new WebSocket(wsUrl)
    wsRef.current = ws

    ws.onopen = () => {
      console.log('WebSocket connected')
      onOpenRef.current?.()
    }

    ws.onmessage = (event) => {
      try {
        const data = JSON.parse(event.data)
        onMessageRef.current?.(data)
      } catch (error) {
        console.error('Failed to parse WebSocket message:', error)
      }
    }

    ws.onclose = () => {
      console.log('WebSocket closed')
      if (!shouldReconnectRef.current) {
        return
      }

      onCloseRef.current?.()
      // Reconnect after 5 seconds
      reconnectTimeoutRef.current = setTimeout(connect, 5000)
    }

    ws.onerror = (error) => {
      console.error('WebSocket error:', error)
    }
  }, [])

  useEffect(() => {
    shouldReconnectRef.current = true
    connect()

    return () => {
      shouldReconnectRef.current = false
      if (reconnectTimeoutRef.current) {
        clearTimeout(reconnectTimeoutRef.current)
      }
      if (wsRef.current) {
        wsRef.current.close()
        wsRef.current = null
      }
    }
  }, [connect])

  const send = useCallback((message) => {
    if (wsRef.current && wsRef.current.readyState === WebSocket.OPEN) {
      wsRef.current.send(message)
    } else {
      console.warn('WebSocket is not connected')
    }
  }, [])

  return { send, ws: wsRef.current }
}
