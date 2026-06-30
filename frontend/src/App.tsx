import { CssBaseline, Box } from '@mui/material'
import { Header } from './components/Header.tsx';
import { Matchlist } from './pages/matchlist/Matchlist.tsx';
import { LocalizationProvider } from '@mui/x-date-pickers';
import { AdapterDayjs } from '@mui/x-date-pickers/AdapterDayjs';

function App() {
  return (
    <LocalizationProvider dateAdapter={AdapterDayjs}>
    <CssBaseline />
      <Box style={{ backgroundColor: 'var(--color-tan)', minHeight: '100vh' }}>
      <Header />
      <Matchlist />
    </Box>
    </LocalizationProvider>
  )
}

export default App
