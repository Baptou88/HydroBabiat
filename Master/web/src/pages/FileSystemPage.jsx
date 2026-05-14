import { useEffect, useMemo, useState } from 'preact/hooks'
import { SectionCard } from '../components/SectionCard'

export function FileSystemPage() {
  const [files, setFiles] = useState([])
  const [uploadProgress, setUploadProgress] = useState(0)
  const [isUploading, setIsUploading] = useState(false)
  const [status, setStatus] = useState('')

  const refresh = async () => {
    const response = await fetch('/api/fs')
    const payload = await response.json()
    const withIds = (payload.SPIFFS || []).map((file, index) => ({ ...file, id: index }))
    setFiles(withIds)
  }

  useEffect(() => {
    refresh().catch(() => setStatus('Impossible de charger le contenu SPIFFS'))
  }, [])

  const summary = useMemo(() => {
    const totalSize = files.reduce((acc, file) => acc + Number(file.size || 0), 0)
    return {
      count: files.length,
      totalSize,
      averageSize: files.length ? totalSize / files.length : 0
    }
  }, [files])

  const sendFile = async (file, ota = false) => {
    const formData = new FormData()
    formData.append('fileName', file.path)
    if (ota) {
      formData.append('Ota', true)
    }

    const response = await fetch('/sendFile', {
      method: 'POST',
      body: formData
    })

    setStatus(response.ok ? `${file.name} envoye` : `Echec envoi ${file.name}`)
  }

  const deleteFile = async (file) => {
    const formData = new FormData()
    formData.append('fileName', file.path)

    const response = await fetch('/api/fs', {
      method: 'DELETE',
      body: formData
    })

    if (response.ok) {
      setFiles((current) => current.filter((item) => item.path !== file.path))
      setStatus(`${file.name} supprime`)
    }
  }

  const uploadFiles = (event) => {
    event.preventDefault()
    const form = event.currentTarget
    const input = form.querySelector('input[type="file"]')
    if (!input?.files?.length) {
      return
    }

    const formData = new FormData(form)
    const xhr = new XMLHttpRequest()
    xhr.open('POST', '/fileSystem')
    setIsUploading(true)
    setUploadProgress(0)
    setStatus('')

    xhr.upload.onprogress = (progressEvent) => {
      if (!progressEvent.lengthComputable) {
        return
      }
      setUploadProgress(Math.round((progressEvent.loaded / progressEvent.total) * 100))
    }

    xhr.onload = async () => {
      setIsUploading(false)
      setStatus(xhr.status >= 200 && xhr.status < 300 ? 'Upload termine' : 'Echec upload')
      await refresh()
      form.reset()
    }

    xhr.onerror = () => {
      setIsUploading(false)
      setStatus('Erreur reseau pendant upload')
    }

    xhr.send(formData)
  }

  return (
    <div class="dashboard-grid dashboard-grid-2">
      <SectionCard title="Synthese SPIFFS" subtitle="Volume de fichiers" icon="bi bi-device-hdd">
        <div class="stat-grid compact">
          <Metric label="Fichiers" value={String(summary.count)} />
          <Metric label="Total" value={humanReadableSize(summary.totalSize)} />
          <Metric label="Taille moyenne" value={humanReadableSize(summary.averageSize)} />
        </div>
        <div class="button-row compact">
          <button type="button" class="btn btn-outline-primary" onClick={refresh}>Rafraichir</button>
        </div>
        {status ? <p class="status-text">{status}</p> : null}
      </SectionCard>

      <SectionCard title="Upload" subtitle="Ajout de fichiers" icon="bi bi-cloud-arrow-up">
        <form class="upload-form" onSubmit={uploadFiles}>
          <input class="form-control" type="file" name="file" multiple />
          <button type="submit" class="btn btn-primary" disabled={isUploading}>
            {isUploading ? 'Upload...' : 'Envoyer vers SPIFFS'}
          </button>
        </form>
        <div class="progress hydro-progress">
          <div class="progress-bar" role="progressbar" style={{ width: `${uploadProgress}%` }}>
            {uploadProgress}%
          </div>
        </div>
      </SectionCard>

      <SectionCard title="Fichiers" subtitle="Operations directes" icon="bi bi-folder-symlink" className="dashboard-span-2">
        <div class="table-responsive">
          <table class="table align-middle hydro-table">
            <thead>
              <tr>
                <th>ID</th>
                <th>Nom</th>
                <th>Chemin</th>
                <th>Taille</th>
                <th>Actions</th>
              </tr>
            </thead>
            <tbody>
              {files.map((file) => (
                <tr key={file.path}>
                  <td>{file.id}</td>
                  <td>{file.name}</td>
                  <td>{file.path}</td>
                  <td>{humanReadableSize(file.size)}</td>
                  <td>
                    <div class="button-row compact nowrap">
                      <button type="button" class="btn btn-sm btn-outline-success" onClick={() => sendFile(file, false)}>Send</button>
                      <button type="button" class="btn btn-sm btn-outline-warning" onClick={() => sendFile(file, true)}>Send OTA</button>
                      <button type="button" class="btn btn-sm btn-outline-danger" onClick={() => deleteFile(file)}>Delete</button>
                    </div>
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </SectionCard>
    </div>
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

function humanReadableSize(bytes, decimals = 2) {
  const normalized = Number(bytes || 0)
  if (normalized === 0) {
    return '0 Bytes'
  }

  const k = 1024
  const dm = decimals < 0 ? 0 : decimals
  const sizes = ['Bytes', 'kB', 'MB', 'GB', 'TB']
  const index = Math.floor(Math.log(normalized) / Math.log(k))
  return `${parseFloat((normalized / Math.pow(k, index)).toFixed(dm))} ${sizes[index]}`
}
