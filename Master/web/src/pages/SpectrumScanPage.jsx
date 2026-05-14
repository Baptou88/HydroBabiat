import { useEffect, useRef, useState } from 'preact/hooks'
import { SectionCard } from '../components/SectionCard'

const SCAN_MARK_START = 'SCAN '
const SCAN_MARK_END = ', END'
const Y_AXIS = [-11, -15, -19, -23, -27, -31, -35, -39, -43, -47, -51, -55, -59, -63, -67, -71, -75, -79, -83, -87, -91, -95, -99, -103, -107, -111, -115, -119, -123, -127, -131, -135, -139]

export function SpectrumScanPage() {
  const chartElementRef = useRef(null)
  const chartRef = useRef(null)
  const scanCountRef = useRef(0)
  const pointsRef = useRef([])
  const [rawLine, setRawLine] = useState('SCAN 0,0,0,0,0,0,0,0,1792,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,12,0, END')

  useEffect(() => {
    if (!window.Highcharts || !chartElementRef.current) {
      return
    }

    chartRef.current = window.Highcharts.chart(chartElementRef.current, {
      chart: { type: 'heatmap', plotBorderWidth: 1, className: 'hydro-highchart' },
      title: { text: 'RadioLib SX126X Spectral Scan' },
      xAxis: { categories: [] },
      yAxis: { categories: Y_AXIS, title: { text: 'RSSI (dBm)' }, reversed: true },
      colorAxis: {
        min: 0,
        minColor: '#cbece4',
        maxColor: '#0c5b57'
      },
      legend: { align: 'right', layout: 'vertical', verticalAlign: 'top', y: 24, symbolHeight: 260 },
      plotOptions: { series: { turboThreshold: 0 } },
      series: [{ name: 'Scan', borderWidth: 0.2, data: [] }]
    })

    return () => chartRef.current?.destroy()
  }, [])

  const addRandomScan = () => {
    const points = []
    for (let index = 0; index < 33; index += 1) {
      points.push([scanCountRef.current, index, Math.floor(Math.random() * 2048)])
    }

    appendPoints(points)
  }

  const addParsedScan = () => {
    const values = parseScanLine(rawLine)
    if (!values.length) {
      return
    }

    appendPoints(values.map((value, index) => [scanCountRef.current, index, value]))
  }

  const appendPoints = (points) => {
    const chart = chartRef.current
    if (!chart) {
      return
    }

    const categories = chart.xAxis[0].categories || []
    chart.xAxis[0].setCategories([...categories, String(scanCountRef.current + 1)], false)
    pointsRef.current = [...pointsRef.current, ...points]
    chart.series[0].setData(pointsRef.current, true)
    scanCountRef.current += 1
  }

  return (
    <div class="dashboard-grid dashboard-grid-2">
      <SectionCard title="Heatmap" subtitle="Visualisation radio" icon="bi bi-broadcast-pin" className="dashboard-span-2">
        <div ref={chartElementRef} class="chart-host chart-host-tall"></div>
      </SectionCard>

      <SectionCard title="Injection" subtitle="Jeu de donnees test" icon="bi bi-bezier2">
        <div class="button-row">
          <button type="button" class="btn btn-primary" onClick={addRandomScan}>Ajouter scan aleatoire</button>
          <button type="button" class="btn btn-outline-secondary" onClick={addParsedScan}>Ajouter scan parse</button>
        </div>
        <label class="field-stack mt-3">
          <span>Ligne brute</span>
          <textarea class="form-control" rows="4" value={rawLine} onInput={(event) => setRawLine(event.target.value)}></textarea>
        </label>
      </SectionCard>
    </div>
  )
}

function parseScanLine(line) {
  if (!line.startsWith(SCAN_MARK_START) || !line.endsWith(SCAN_MARK_END)) {
    return []
  }

  return line
    .replace(SCAN_MARK_START, '')
    .replace(SCAN_MARK_END, '')
    .split(',')
    .map((item) => Number(item.trim()))
    .filter((item) => !Number.isNaN(item))
}
