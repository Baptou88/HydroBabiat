import { useEffect, useRef } from 'preact/hooks'

export function ChartPanel({ chartsRef, etang, turbine }) {
  const niveauChartRef = useRef(null)
  const turbineChartRef = useRef(null)
  const internalChartsRef = useRef({})
  const chartStoreRef = chartsRef ?? internalChartsRef

  useEffect(() => {
    if (window.Highcharts && niveauChartRef.current && !chartStoreRef.current?.chartNiveau) {
      const niveauOptions = {
        chart: {
          type: 'area',
          styledMode: true,
          height: 300
        },
        title: { text: 'Niveau Étang (%)' },
        xAxis: { type: 'datetime' },
        yAxis: {
          min: -10,
          max: 110,
          title: { text: '(%)' }
        },
        series: [{
          name: 'Niveau (%)',
          data: []
        }],
        rangeSelector: {
          buttons: [
            { count: 1, type: 'minute', text: '1m' },
            { count: 5, type: 'minute', text: '5m' },
            { count: 30, type: 'minute', text: '30m' },
            { count: 1, type: 'day', text: '1d' },
            { count: 1, type: 'week', text: '1w' },
            { type: 'all', text: 'Tout' }
          ],
          selected: 0
        },
        legend: { enabled: true },
        plotOptions: {
          series: { showInNavigator: true }
        }
      }

      chartStoreRef.current = chartStoreRef.current || {}
      chartStoreRef.current.chartNiveau = window.Highcharts.stockChart('chartNiveau', niveauOptions)
    }

    if (window.Highcharts && turbineChartRef.current && !chartStoreRef.current?.chartTurbine) {
      const turbineOptions = {
        chart: {
          type: 'area',
          styledMode: true,
          height: 300
        },
        title: { text: 'Turbine (%)' },
        xAxis: { type: 'datetime' },
        yAxis: [{
          title: { text: 'Ouverture (%)' },
          min: 0,
          max: 100
        }, {
          title: { text: 'Tacky (rpm)' },
          opposite: true
        }],
        series: [
          {
            name: 'Ouverture (%)',
            data: [],
            color: '#05918a',
            yAxis: 0
          },
          {
            name: 'Cible (%)',
            data: [],
            color: '#FFAA8a',
            yAxis: 0
          },
          {
            name: 'Tacky (rpm)',
            data: [],
            color: '#ee931f',
            yAxis: 1
          }
        ],
        rangeSelector: {
          buttons: [
            { count: 1, type: 'minute', text: '1m' },
            { count: 5, type: 'minute', text: '5m' },
            { count: 30, type: 'minute', text: '30m' },
            { count: 1, type: 'day', text: '1d' },
            { count: 1, type: 'week', text: '1w' },
            { type: 'all', text: 'Tout' }
          ],
          selected: 0
        },
        legend: { enabled: true },
        plotOptions: {
          series: { showInNavigator: true }
        }
      }

      chartStoreRef.current.chartTurbine = window.Highcharts.stockChart('chartTurbine', turbineOptions)
    }
  }, [chartStoreRef])

  useEffect(() => {
    const dt = Date.now()
    if (chartStoreRef.current?.chartNiveau && etang?.niveauEtangP !== undefined) {
      addPointIfSeriesExists(chartStoreRef.current.chartNiveau, 0, [dt, etang.niveauEtangP])
    }
  }, [chartStoreRef, etang?.niveauEtangP])

  useEffect(() => {
    const dt = Date.now()
    if (chartStoreRef.current?.chartTurbine) {
      if (turbine?.positionVanne !== undefined) {
        addPointIfSeriesExists(chartStoreRef.current.chartTurbine, 0, [dt, turbine.positionVanne])
      }
      if (turbine?.PositionVanneTarget !== undefined) {
        addPointIfSeriesExists(chartStoreRef.current.chartTurbine, 1, [dt, turbine.PositionVanneTarget])
      }
      if (turbine?.tacky !== undefined) {
        addPointIfSeriesExists(chartStoreRef.current.chartTurbine, 2, [dt, turbine.tacky])
      }
    }
  }, [chartStoreRef, turbine?.positionVanne, turbine?.PositionVanneTarget, turbine?.tacky])

  return (
    <div class="row g-3">
      <div class="col-12">
        <div class="card">
          <div class="card-body p-0">
            <div id="chartNiveau" ref={niveauChartRef} style={{ height: '400px' }}></div>
          </div>
        </div>
      </div>
      <div class="col-12">
        <div class="card">
          <div class="card-body p-0">
            <div id="chartTurbine" ref={turbineChartRef} style={{ height: '400px' }}></div>
          </div>
        </div>
      </div>
    </div>
  )
}

function addPointIfSeriesExists(chart, index, point) {
  if (!chart || !Array.isArray(chart.series)) {
    return
  }
  const serie = chart.series[index]
  if (serie && typeof serie.addPoint === 'function') {
    serie.addPoint(point, true, false, true)
  }
}
