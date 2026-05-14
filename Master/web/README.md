# HydroBabiat Web Interface

Interface web moderne pour HydroBabiat basée sur Preact, construite avec Vite.

## 🚀 Démarrage rapide

### Installation des dépendances

```bash
cd Master/web
npm install
```

### Développement

```bash
npm run dev
```

Ouvre automatiquement http://localhost:3000 dans votre navigateur.

### Build et Déploiement

Pour compiler l'interface et la copier automatiquement dans `Master/data` :

```bash
npm run deploy
```

Ou manuellement :

```bash
npm run build    # Compile dans dist/
npm run deploy   # Copie dans Master/data et nettoie
```

## 📁 Structure du projet

```
web/
├── src/
│   ├── main.jsx              # Point d'entrée Preact
│   ├── App.jsx               # Composant principal
│   ├── style.css             # Styles globaux
│   ├── index.html            # Template HTML
│   ├── components/           # Composants réutilisables
│   │   ├── Navbar.jsx        # Barre de navigation
│   │   ├── DataPanel.jsx     # Affichage des données
│   │   ├── ChartPanel.jsx    # Graphiques Highcharts
│   │   └── TerminalPanel.jsx # Terminal pour commandes
│   └── hooks/                # Custom hooks
│       └── useWebSocket.js   # Gestion WebSocket
├── package.json              # Dépendances npm
├── vite.config.js           # Config Vite
├── postBuild.js             # Script post-build
└── README.md                # Ce fichier
```

## 🔧 Configuration

### Variables d'environnement

Créez un fichier `.env.local` si besoin :

```env
VITE_API_URL=ws://localhost/ws
```

### Vite Configuration

La build est configurée pour :
- Sortir dans `../data` (directement accessible par l'ESP32)
- Minifier le code
- Générer `app.js` comme fichier principal

## 📦 Dépendances principales

- **Preact** - Alternative légère à React
- **Vite** - Bundler ultrarapide
- **Bootstrap 5** - Framework CSS
- **Highcharts** - Graphiques en temps réel

## 🌐 Intégration WebSocket

Le hook `useWebSocket` gère automatiquement :
- Connexion au serveur WebSocket
- Réconnexion automatique après 5s en cas de déconnexion
- Parsing des messages JSON
- Callbacks pour open/close/message

### Utilisation

```jsx
const { send } = useWebSocket(
  (message) => { /* onMessage */ },
  () => { /* onOpen */ },
  () => { /* onClose */ }
)

send('Action:TURBINE:positionVanne=50;')
```

## 🎨 Styles

Les styles Bootstrap sont chargés via CDN. Personnalisez via :
- `src/style.css` - Styles personnalisés
- Thème Bootstrap (clair/sombre) géré par `data-bs-theme`

## 📊 Graphiques

Les graphiques Highcharts sont initialisés dans `ChartPanel` avec :
- Range selector (1m, 5m, 30m, 1d, 1w, Tout)
- Axes multiples pour Turbine
- Points en temps réel via WebSocket

## 🔐 Sécurité

- Pas de données sensibles en dur
- CSP header recommandé
- Certificats SSL dans `Master/web/` (Cert.pem, Key.pem)

## 🛠️ Développement des composants

### Ajouter un nouveau composant

1. Créer le fichier dans `src/components/MonComposant.jsx`
2. Importer et utiliser dans `App.jsx`
3. Utiliser les hooks de Preact : `useEffect`, `useState`, `useRef`

Exemple :

```jsx
import { useState } from 'preact/hooks'

export function MonComposant() {
  const [count, setCount] = useState(0)
  
  return (
    <div class="card">
      <button onClick={() => setCount(count + 1)}>
        Compteur: {count}
      </button>
    </div>
  )
}
```

## 📝 Notes

- L'ancien interface (HTML statique) dans `Master/web/` reste pour compatibilité
- Les données en temps réel viennent via WebSocket depuis l'ESP32
- Les graphiques supportent zoom/pan nativement via Highcharts

## 🐛 Dépannage

### WebSocket ne se connecte pas
- Vérifier que l'ESP32 publie correctement sur `/ws`
- Vérifier la console navigateur pour les erreurs

### Build fails
- Vérifier Node.js v16+ installé : `node --version`
- Supprimer `node_modules` et `npm install` de nouveau

### Styles ne se chargent pas
- Vérifier que Bootstrap CDN est accessible
- Vérifier les chemins relatifs dans HTML

## 📚 Ressources

- [Preact Docs](https://preactjs.com)
- [Vite Docs](https://vitejs.dev)
- [Bootstrap 5](https://getbootstrap.com)
- [Highcharts](https://www.highcharts.com)
