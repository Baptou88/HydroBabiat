import fs from 'fs'
import path from 'path'
import { fileURLToPath } from 'url'

const __filename = fileURLToPath(import.meta.url)
const __dirname = path.dirname(__filename)

const webDir = __dirname
const dataDir = path.resolve(__dirname, '../data')
const legacyAssets = ['app.js', 'theme.js', 'fileSystem.js', 'Programmateur.js', 'style.css']

console.log('📦 Post-build: Copying assets to data folder...')

try {
  // Ensure data directory exists
  if (!fs.existsSync(dataDir)) {
    fs.mkdirSync(dataDir, { recursive: true })
  }

  // Copy dist files to data
  const distDir = path.join(webDir, 'dist')
  if (fs.existsSync(distDir)) {
    const files = fs.readdirSync(distDir)
    
    files.forEach(file => {
      const src = path.join(distDir, file)
      const dest = path.join(dataDir, file)
      
      if (fs.statSync(src).isFile()) {
        fs.copyFileSync(src, dest)
        console.log(`✓ Copied: ${file}`)
      }
    })
  }

  // Remove dist folder
  if (fs.existsSync(distDir)) {
    fs.rmSync(distDir, { recursive: true, force: true })
    console.log('✓ Cleaned dist folder')
  }

  legacyAssets.forEach((file) => {
    const target = path.join(dataDir, file)
    if (fs.existsSync(target)) {
      fs.rmSync(target, { force: true })
      console.log(`✓ Removed legacy asset: ${file}`)
    }
  })

  console.log('✅ Build deployment complete!')
} catch (error) {
  console.error('❌ Post-build error:', error)
  process.exit(1)
}
