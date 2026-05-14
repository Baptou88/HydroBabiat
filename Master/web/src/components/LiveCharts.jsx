import { useEffect, useRef } from 'preact/hooks'
import { SectionCard } from './SectionCard'

export function LiveCharts({ activeMode, etang, turbine }) {
  const niveauElementRef = useRef(null)
  const turbineElementRef = useRef(null)
  const chartsRef = useRef({ chartNiveau: null, chartTurbine: null })

  useEffect(() => {
    if (!window.Highcharts || !niveauElementRef.current || !turbineElementRef.current) {
      return
    }

    const rangeSelector = {
      buttons: [
        { count: 1, type: 'minute', text: '1M' },
        { count: 5, type: 'minute', text: '5M' },
        { count: 30, type: 'minute', text: '30M' },
        { count: 1, type: 'day', text: '1J' },
        { count: 1, type: 'week', text: '1S' },
        { type: 'all', text: 'Tout' }
      ],
      inputEnabled: false,
      selected: 0
    }

    chartsRef.current.chartNiveau = window.Highcharts.stockChart(niveauElementRef.current, {
      chart: { className: 'hydro-highchart', backgroundColor: 'transparent' },
      time: { useUTC: false },
      rangeSelector,
      title: { text: 'Niveau etang' },
      xAxis: { type: 'datetime' },
      yAxis: { min: -10, max: 110, title: { text: '(%)' } },
      legend: { enabled: true },
      plotOptions: { series: { showInNavigator: true, turboThreshold: 0 } },
      series: [{ name: 'Niveau (%)', data: [] }]
    })

    chartsRef.current.chartTurbine = window.Highcharts.stockChart(turbineElementRef.current, {
      chart: { className: 'hydro-highchart', backgroundColor: 'transparent' },
      time: { useUTC: false },
      rangeSelector,
      title: { text: 'Turbine' },
      xAxis: { type: 'datetime' },
      yAxis: [
        { title: { text: '(%)' } },
        { title: { text: 'tacky' }, opposite: true }
      ],
      legend: { enabled: true },
      plotOptions: { series: { showInNavigator: true, turboThreshold: 0 } },
      series: [
        { name: 'Ouverture (%)', data: [], color: '#169873' },
        { name: 'Cible (%)', data: [], color: '#ff9f68' },
        { name: 'Tacky (rpm)', data: [], color: '#f2b134', yAxis: 1 }
      ]
    })

    fetch('/data.csv')
      .then((response) => response.ok ? response.text() : '')
      .then((csv) => {
        if (!csv) {
          return
        }

        const lines = csv.split('\n').filter(Boolean)
        lines.slice(1).forEach((line) => {
          const items = line.split(',')
          const dt = new Date((Number(items[0]) - 3600) * 1000).getTime()
          addPointIfSeriesExists(chartsRef.current.chartNiveau, 0, [dt, Number(items[2])], false)
          addPointIfSeriesExists(chartsRef.current.chartTurbine, 0, [dt, Number(items[4])], false)
          addPointIfSeriesExists(chartsRef.current.chartTurbine, 1, [dt, Number(items[3])], false)
          addPointIfSeriesExists(chartsRef.current.chartTurbine, 2, [dt, Number(items[1])], false)
        })

        chartsRef.current.chartNiveau?.redraw()
        chartsRef.current.chartTurbine?.redraw()
      })
      .catch(() => {})

    return () => {
      chartsRef.current.chartNiveau?.destroy()
      chartsRef.current.chartTurbine?.destroy()
      chartsRef.current.chartNiveau = null
      chartsRef.current.chartTurbine = null
    }
  }, [])

  useEffect(() => {
    const chart = chartsRef.current.chartNiveau
    if (!chart) {
      return
    }

    chart.yAxis[0].removePlotBand('basic-band')
    if (activeMode === 1) {
      chart.yAxis[0].addPlotBand({ from: 20, to: 80, color: '#bdf5d6', id: 'basic-band' })
    }
  }, [activeMode])

  useEffect(() => {
    if (etang?.niveauEtangP === undefined) {
      return
    }

    addPointIfSeriesExists(chartsRef.current.chartNiveau, 0, [Date.now(), Number(etang.niveauEtangP)])
  }, [etang?.niveauEtangP])

  useEffect(() => {
    const now = Date.now()
    if (turbine?.positionVanne !== undefined) {
      addPointIfSeriesExists(chartsRef.current.chartTurbine, 0, [now, Number(turbine.positionVanne)])
    }
    if (turbine?.PositionVanneTarget !== undefined) {
      addPointIfSeriesExists(chartsRef.current.chartTurbine, 1, [now, Number(turbine.PositionVanneTarget)])
    }
    if (turbine?.tacky !== undefined) {
      addPointIfSeriesExists(chartsRef.current.chartTurbine, 2, [now, Number(turbine.tacky)])
    }
  }, [turbine?.positionVanne, turbine?.PositionVanneTarget, turbine?.tacky])

  return (
    <div class="chart-grid">
      <SectionCard title="Courbe niveau" subtitle="Historique etang" icon="bi bi-graph-up-arrow">
        <div ref={niveauElementRef} class="chart-host"></div>
      </SectionCard>
      <SectionCard title="Courbe turbine" subtitle="Ouverture et tacky" icon="bi bi-activity">
        <div ref={turbineElementRef} class="chart-host"></div>
      </SectionCard>
    </div>
  )
}

function addPointIfSeriesExists(chart, index, point, redraw = true) {
  if (!chart || !Array.isArray(chart.series)) {
    return
  }

  if (!Array.isArray(point) || point.length < 2) {
    return
  }

  const x = Number(point[0])
  const y = Number(point[1])
  if (!Number.isFinite(x) || !Number.isFinite(y)) {
    return
  }

  const serie = chart.series[index]
  if (serie && typeof serie.addPoint === 'function') {
    serie.addPoint([x, y], redraw, false, false)
  }
}
