import { Typography, Stack } from '@mui/material'
import { styled } from '@mui/material/styles';
import goldicon from '../assets/gold_icon.png';

const HeaderRowContainer = styled(Stack)`
  background-color: var(--color-maroon);
  margin-bottom: 22px;
  border-bottom: 2px solid var(--color-brown);
`;
const HeaderText = styled(Typography)`
  font-family: 'Albertson';
  font-weight: normal;
  font-style: normal;
  font-size: 32px;
  color: var(--color-yellow);
  text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.7);
`;

export const Header = () => {
  return (
    <HeaderRowContainer direction="row" spacing={2}>
      <img src={goldicon} style={{
        width: 'auto',
        height: '32px',
        marginLeft: '16px',
        marginTop: '6px',
        marginBottom: 'auto',
        imageRendering: 'pixelated'
      }} />
      <HeaderText>Gold Rush Replays</HeaderText>
    </HeaderRowContainer>
  )
};
