export function DataPanel({ etang, turbine, radiateur, onSendAction }) {
  return (
    <div class="row g-3">
      {/* Etang Section */}
      <div class="col-12">
        <div class="card">
          <div class="card-header">
            <h5 class="mb-0">
              <i class="bi bi-water"></i> Étang
            </h5>
          </div>
          <div class="card-body">
            <div class="row g-3">
              <div class="col-md-6">
                <div class="input-group">
                  <span class="input-group-text">Niveau Rempli</span>
                  <input 
                    type="number" 
                    class="form-control"
                    value={etang?.niveauRempli || 0}
                    onChange={(e) => onSendAction('ETANG', `setNiveauFull=${e.target.value}`)}
                  />
                </div>
              </div>
              <div class="col-md-6">
                <div class="input-group">
                  <span class="input-group-text">Niveau Vide</span>
                  <input 
                    type="number" 
                    class="form-control"
                    value={etang?.niveauVide || 0}
                    onChange={(e) => onSendAction('ETANG', `setNiveauEmpty=${e.target.value}`)}
                  />
                </div>
              </div>
              <div class="col-12">
                <div class="card bg-light">
                  <div class="card-body text-center">
                    <h6>Niveau Actuel</h6>
                    <h3 class="mb-0">{(etang?.niveauEtangP || 0).toFixed(1)}%</h3>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>

      {/* Turbine Section */}
      <div class="col-12">
        <div class="card">
          <div class="card-header">
            <h5 class="mb-0">
              <i class="bi bi-arrow-clockwise"></i> Turbine
            </h5>
          </div>
          <div class="card-body">
            <div class="row g-3">
              <div class="col-md-6">
                <label class="form-label">Position Vanne</label>
                <input 
                  type="range" 
                  class="form-range"
                  min="0" 
                  max="100"
                  value={turbine?.positionVanne || 0}
                  onChange={(e) => onSendAction('TURBINE', `positionVanne=${e.target.value}`)}
                />
                <small class="text-muted">{(turbine?.positionVanne || 0).toFixed(1)}%</small>
              </div>
              <div class="col-md-6">
                <label class="form-label">Puissance</label>
                <div class="alert alert-info mb-0">
                  <strong>{(turbine?.power || 0).toFixed(0)} W</strong>
                </div>
              </div>
              <div class="col-md-6">
                <label class="form-label">Intensité</label>
                <div class="alert alert-warning mb-0">
                  <strong>{(turbine?.intensite || 0).toFixed(2)} A</strong>
                </div>
              </div>
              <div class="col-md-6">
                <label class="form-label">Tension</label>
                <div class="alert alert-success mb-0">
                  <strong>{(turbine?.tension || 0).toFixed(2)} V</strong>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>

      {/* Radiateur Section */}
      <div class="col-12">
        <div class="card">
          <div class="card-header">
            <h5 class="mb-0">
              <i class="bi bi-thermometer-half"></i> Radiateur
            </h5>
          </div>
          <div class="card-body">
            <div class="row g-3">
              <div class="col-md-6">
                <div class="alert alert-info">
                  Temp: <strong>{(radiateur?.temp || 0).toFixed(1)}°C</strong>
                </div>
              </div>
              <div class="col-md-6">
                <div>
                  <input 
                    type="checkbox" 
                    class="form-check-input"
                    checked={radiateur?.Rad1 || false}
                    disabled
                  />
                  <label class="form-check-label">Radiateur 1</label>
                </div>
              </div>
              <div class="col-md-6">
                <div>
                  <input 
                    type="checkbox" 
                    class="form-check-input"
                    checked={radiateur?.Rad2 || false}
                    disabled
                  />
                  <label class="form-check-label">Radiateur 2</label>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  )
}
