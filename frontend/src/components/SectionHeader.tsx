import { Typography, Box } from '@mui/material';
import { styled } from '@mui/material/styles';

export const SectionHeader = styled(Typography)`
  font-family: 'Chango', serif;
  font-weight: 400;
  font-style: normal;
  font-size: 24px;
  color: var(--color-offblack);
  letter-spacing: normal;
`;

export const Section = styled(Box)`
  margin-left: 12px;
  margin-bottom: 12px;
`;
