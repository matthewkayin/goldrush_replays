import { CssBaseline, Box } from '@mui/material'
import { Header } from './components/Header.tsx';
import { Matchlist } from './pages/matchlist/Matchlist.tsx';


function App() {
  return (
    <>
    <CssBaseline />
      <Box style={{ backgroundColor: 'var(--color-tan)', minHeight: '100vh' }}>
      <Header />
      <Matchlist />
    </Box>
    </>
  )
}

export default App
