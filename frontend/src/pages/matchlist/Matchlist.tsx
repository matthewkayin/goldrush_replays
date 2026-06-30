import { Section, SectionHeader, MaroonButton } from '../../components';
import { useState, useCallback, useEffect } from 'react';
import { Container, Snackbar } from '@mui/material';
import { type Match } from '../../types/match.ts';
import { UploadDialog, MatchlistTable, MatchlistFiltersCluster, type MatchlistFilters } from './components';
import { apiGet, apiPost } from '../../api/client.ts';

export const Matchlist = () => {
  const [uploadDialogOpen, setUploadDialogOpen] = useState(false);
  const [data, setData] = useState<Match[]>([]);
  const [filters, setFilters] = useState<MatchlistFilters>({});
  const [queryString, setQueryString] = useState('');
  const [snackMessage, setSnackMessage] = useState('');

  const updateFilters = (newFilters: MatchlistFilters, submit: boolean) => {
    setFilters(newFilters);

    if (submit) {
      const toSnakeCase = str => str.replace(/[A-Z]/g, letter => `_${letter.toLowerCase()}`);
      const queryStringParts = [];
      Object.keys(newFilters).forEach((key) => {
        if (!newFilters[key]) {
          return;
        }

        queryStringParts.push(`${toSnakeCase(key)}=${newFilters[key]}`);
      })

      if (queryStringParts.length === 0) {
        setQueryString('');
        return;
      }

      setQueryString(`?${queryStringParts.join('&')}`);
    }
  };

  // GET MATCH DATA
  useEffect(() => {
    // Cleans up pending requests if the component unmounts
    const abortController = new AbortController();

    const fetchData = async () => {
      try {
        const response = await apiGet<Match>(`/match${queryString}`);
        setData(response.data);
      } catch (err) {
        if (err.name !== 'AbortError') {
          console.log(err.message);
        }
      }
    };

    fetchData();
    return () => abortController.abort();
  }, [queryString]);

  // SUBMIT
  const onSubmit = useCallback(async (files: File[]) => {
    const formData = new FormData();
    files.forEach((file: File) => {
      formData.append('files', file);
    });

    try {
      const response = await apiPost<Match>('/match', {
        body: formData
      });
      const ids = response.data.map((record) => record.id).join(',');
      updateFilters({}, false);
      setQueryString(`?ids=${ids}`);
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
      <MatchlistFiltersCluster
        filters={filters}
        onFiltersUpdated={updateFilters}
      />
      <Section>
        <MaroonButton onClick={(e) => { e.preventDefault(); setUploadDialogOpen(true); }}>⬆️Upload New Replay</MaroonButton>
      </Section>
      <MatchlistTable data={data} />

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
