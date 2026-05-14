import { useEffect, useState } from 'preact/hooks'

function getPreferredTheme() {
  const storedTheme = localStorage.getItem('theme')
  if (storedTheme) {
    return storedTheme
  }

  return window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light'
}

function applyTheme(theme) {
  const resolvedTheme = theme === 'auto'
    ? (window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light')
    : theme

  document.documentElement.setAttribute('data-bs-theme', resolvedTheme)
}

export function useTheme() {
  const [theme, setTheme] = useState(() => getPreferredTheme())

  useEffect(() => {
    applyTheme(theme)
    localStorage.setItem('theme', theme)
  }, [theme])

  useEffect(() => {
    const media = window.matchMedia('(prefers-color-scheme: dark)')
    const handleChange = () => {
      if (localStorage.getItem('theme') === 'auto') {
        applyTheme('auto')
      }
    }

    media.addEventListener('change', handleChange)
    return () => media.removeEventListener('change', handleChange)
  }, [])

  return { theme, setTheme }
}
