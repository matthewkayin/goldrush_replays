import { Button } from '@mui/material';
import { styled } from '@mui/material/styles';

const StyledButton = styled(Button)`
  background-color: var(--color-maroon);
  border: 1px solid var(--color-offblack);
  border-radius: 1px;
  color: var(--color-yellow);
`;

export const MaroonButton = ({ children, ...props }) => {
  return (
    <StyledButton sx={{
      ':hover': {
        backgroundColor: '#6b4340'
      }
    }} {...props}>
      {children}
    </StyledButton>
  );
}
