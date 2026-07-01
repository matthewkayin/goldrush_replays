import { Stack, Typography, IconButton, Box } from '@mui/material';
import { styled } from '@mui/material/styles';
import KeyboardArrowLeftIcon from '@mui/icons-material/KeyboardArrowLeft';
import KeyboardArrowRightIcon from '@mui/icons-material/KeyboardArrowRight';

const PaginationButton = styled(IconButton)`
  background-color: var(--color-maroon);
  border: 1px solid var(--color-offblack);
  border-radius: 1px;
  color: var(--color-yellow);
  height: 24px;
  width: 24px;
`;

export type PaginationClusterProps = {
  page: number;
  maxPage: number;
  setPage: (newValue: number) => void;
}

export const PaginationCluster = ({ page, maxPage, setPage }: PaginationClusterProps) => {
  return (
    <Box sx={{ display: 'grid', gridTemplateColumns: 'auto max-content' }}>
      <Box/>
      <Stack direction="row" spacing={1} sx={{ alignItems: 'center' }}>
        <PaginationButton disabled={page === 1} size="small" onClick={() => setPage(page - 1)}><KeyboardArrowLeftIcon /></PaginationButton>
        <Typography>Page {page}</Typography>
        <PaginationButton disabled={page === maxPage} size="small" onClick={() => setPage(page + 1)}><KeyboardArrowRightIcon /></PaginationButton>
      </Stack>
    </Box>
  );
};
