import { Section, SectionHeader, MaroonButton } from '../../components';
import { useState, useCallback, useEffect } from 'react';
import { Container, Snackbar, Stack, Box } from '@mui/material';
import { type Replay } from '../../types/replay.ts';
import { PaginationCluster, UploadDialog, ReplaylistTable, ReplaylistFiltersCluster, type ReplaylistFilters } from './components';
import { apiGet, apiPost } from '../../api/client.ts';

const REPLAYLIST_PAGE_SIZE = 8;

const getQueryString = (filters: ReplaylistFilters, page: number) => {
  // Special case, if ids specified, then ignore all other filters and pagination
  // and just query based on the ids
  if (filters.ids) {
    return `?ids=${filters.ids.join(',')}`;
  }

  // Otherwise, build the string based on filters
  const toSnakeCase = str => str.replace(/[A-Z]/g, letter => `_${letter.toLowerCase()}`);
  const queryStringParts = [];
  Object.keys(filters).forEach((key) => {
    if (!filters[key]) {
      return;
    }

    queryStringParts.push(`${toSnakeCase(key)}=${filters[key]}`);
  })

  const queryStringFilters = queryStringParts.join(',');
  return `?${queryStringFilters}&offset=${(page - 1) * REPLAYLIST_PAGE_SIZE}&limit=${REPLAYLIST_PAGE_SIZE}`;
};

const getPageCount = (page: number, recordsRemaining?: number) => {
  if (recordsRemaining === undefined) {
    return 1;
  }

  const totalRecords = (page * REPLAYLIST_PAGE_SIZE) + recordsRemaining;
  let totalPages = Math.floor(totalRecords / REPLAYLIST_PAGE_SIZE);
  if (totalRecords % REPLAYLIST_PAGE_SIZE !== 0) {
    totalPages++;
  }
  console.log(`Page ${page} remaining ${recordsRemaining} total records ${totalRecords} total pages ${totalPages}`);

  return totalPages;
}

export const Replaylist = () => {
  const [uploadDialogOpen, setUploadDialogOpen] = useState(false);
  const [data, setData] = useState<Replay[]>([]);
  const [filters, setFilters] = useState<ReplaylistFilters>({});
  const [draftFilters, setDraftFilters] = useState<ReplaylistFilters>({});
  const [snackMessage, setSnackMessage] = useState('');
  const [page, setPage] = useState(1);
  const [maxPage, setMaxPage] = useState(1);

  const updateFilters = (newFilters: ReplaylistFilters, submit: boolean) => {
    setDraftFilters(newFilters);
    if (submit) {
      setFilters(newFilters);
      setPage(1);
    }
  };

  const updatePage = (newPage: number) => {
    if (newPage < 1 || newPage > maxPage) {
      return;
    }
    setPage(newPage);
  };

  // GET REPLAY DATA
  useEffect(() => {
    // Cleans up pending requests if the component unmounts
    const abortController = new AbortController();

    const queryString = getQueryString(filters, page);

    const fetchData = async () => {
      try {
        const response = await apiGet<Replay>(`/replay${queryString}`);
        setData(response.data);
        setMaxPage(getPageCount(page, response.remaining));
      } catch (err) {
        if (err.name !== 'AbortError') {
          console.log(err.message);
        }
      }
    };

    fetchData();
    return () => abortController.abort();
  }, [filters, page]);

  // SUBMIT
  const onSubmit = useCallback(async (files: File[]) => {
    const formData = new FormData();
    files.forEach((file: File) => {
      formData.append('files', file);
    });

    try {
      const response = await apiPost<Replay>('/replay', {
        body: formData
      });

      updateFilters({
        ids: response.data.map((record) => record.id),
      }, true);

      setSnackMessage('Upload success');
    } catch (err) {
      console.log('Error: ', err);
    }
  }, []);

  // SNACK MESSAGE ON CLOSE
  const onSnackbarCloseCallback = useCallback(() => {
    setSnackMessage('');
  }, []);

  return (
    <Container>
      <Section>
        <SectionHeader>Replays</SectionHeader>
      </Section>
      <Stack spacing={2}>
        <Stack direction="row" spacing={2}>
          <ReplaylistFiltersCluster
            filters={draftFilters}
            onFiltersUpdated={updateFilters}
          />
          <Stack>
            <MaroonButton onClick={(e) => { e.preventDefault(); setUploadDialogOpen(true); }}>⬆️Upload New Replay</MaroonButton>
            <Box/>
          </Stack>
        </Stack>
        <PaginationCluster page={page} maxPage={maxPage} setPage={updatePage} />
        <ReplaylistTable data={data} />
      </Stack>

      <UploadDialog
      open={uploadDialogOpen}
      onClose={() => setUploadDialogOpen(false)}
      onSubmit={onSubmit}
      />
      <Snackbar
      anchorOrigin={{ vertical: 'bottom', horizontal: 'right' }}
      open={snackMessage !== ''}
      onClose={onSnackbarCloseCallback}
      message={snackMessage}
      autoHideDuration={3000}
      />
    </Container>
  );
};
