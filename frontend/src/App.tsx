import classes from './App.module.css'
import { Matchlist } from './pages/matchlist/Matchlist.tsx';

function App() {
  return (
    <>
      <div className={classes.header}>
        <h1 className={classes.title}>Gold Rush Replays</h1>
      </div>
      <Matchlist />
    </>
  )
}

export default App
