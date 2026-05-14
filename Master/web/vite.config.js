import { defineConfig } from 'vite'
import preact from '@preact/preset-vite'
import { readFileSync } from 'fs'
import path from 'path'
import { WebSocketServer } from 'ws'

function devMockWsPlugin() {
  return {
    name: 'dev-mock-ws',
    apply: 'serve',
    configureServer(server) {
      const pageRoutes = {
        '/programmateur': 'programmateur.html',
        '/fileSystem': 'fileSystem.html',
        '/notAuth': 'notAuth.html',
        '/spectrumScan': 'spectrumScan.html'
      }

      server.middlewares.use(async (req, res, next) => {
        const route = req.url?.split('?')[0]
        const htmlFile = pageRoutes[route]
        if (!htmlFile) {
          next()
          return
        }

        const filePath = path.resolve(__dirname, htmlFile)
        const template = readFileSync(filePath, 'utf-8')
        const html = await server.transformIndexHtml(route, template)
        res.setHeader('Content-Type', 'text/html')
        res.end(html)
      })

      const wss = new WebSocketServer({ noServer: true })

      const clamp = (value, min, max) => Math.max(min, Math.min(max, value))
      const randomWalk = (value, step, min, max) => {
        const next = value + (Math.random() * 2 - 1) * step
        return clamp(next, min, max)
      }

      const state = {
        niveauEtangP: 58,
        positionVanne: 42,
        PositionVanneTarget: 47,
        tacky: 860,
        power: 320,
        intensite: 4.8,
        tension: 68,
        temp: 34,
        Energie: 1250
      }

      const interval = setInterval(() => {
        if (wss.clients.size === 0) {
          return
        }

        state.niveauEtangP = randomWalk(state.niveauEtangP, 1.2, 10, 100)
        state.positionVanne = randomWalk(state.positionVanne, 2.5, 0, 100)
        state.PositionVanneTarget = randomWalk(state.PositionVanneTarget, 1.5, 0, 100)
        state.tacky = randomWalk(state.tacky, 25, 200, 1800)
        state.power = randomWalk(state.power, 12, 0, 1200)
        state.intensite = randomWalk(state.intensite, 0.15, 0, 25)
        state.tension = randomWalk(state.tension, 0.35, 10, 90)
        state.temp = randomWalk(state.temp, 0.4, 10, 95)
        state.Energie = state.Energie + Math.max(state.power, 0) / 3600

        const payload = JSON.stringify({
          data: {
            Energie: Number(state.Energie.toFixed(2)),
            Etang: {
              niveauRempli: 80,
              niveauVide: 20,
              niveauEtangP: Number(state.niveauEtangP.toFixed(2)),
              niveauEtang: Number((state.niveauEtangP * 1.5).toFixed(2))
            },
            Turbine: {
              positionVanne: Number(state.positionVanne.toFixed(2)),
              PositionVanneTarget: Number(state.PositionVanneTarget.toFixed(2)),
              tacky: Math.round(state.tacky),
              power: Number(state.power.toFixed(2)),
              intensite: Number(state.intensite.toFixed(2)),
              tension: Number(state.tension.toFixed(2)),
              motorState: 1,
              motorStateStr: 'RUNNING'
            },
            Radiateur: {
              temp: Number(state.temp.toFixed(2)),
              Rad1: state.temp > 38,
              Rad2: state.temp > 45
            }
          }
        })

        for (const client of wss.clients) {
          if (client.readyState === client.OPEN) {
            client.send(payload)
          }
        }
      }, 1000)

      wss.on('connection', (ws) => {
        ws.send(JSON.stringify({ monitor: 'Mock websocket connected (dev mode)' }))
      })

      server.httpServer?.on('upgrade', (request, socket, head) => {
        if (request.url !== '/ws') {
          return
        }

        wss.handleUpgrade(request, socket, head, (ws) => {
          wss.emit('connection', ws, request)
        })
      })

      server.httpServer?.on('close', () => {
        clearInterval(interval)
        wss.close()
      })
    }
  }
}

export default defineConfig({
  plugins: [preact(), devMockWsPlugin()],
  build: {
    outDir: path.resolve(__dirname, '../data'),
    emptyOutDir: false,
    minify: 'esbuild',
    rollupOptions: {
      input: {
        main: path.resolve(__dirname, 'index.html'),
        home: path.resolve(__dirname, 'home.html'),
        fileSystem: path.resolve(__dirname, 'fileSystem.html'),
        programmateur: path.resolve(__dirname, 'programmateur.html'),
        notAuth: path.resolve(__dirname, 'notAuth.html'),
        spectrumScan: path.resolve(__dirname, 'spectrumScan.html')
      },
      output: {
        entryFileNames: 'app.js',
        chunkFileNames: '[name].js',
        assetFileNames: '[name][extname]'
      }
    }
  },
  server: {
    port: 3000,
    open: true
  }
})
