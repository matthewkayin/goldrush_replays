import { Box, Stack, Typography, TextField } from '@mui/material';
import { DatePicker } from '@mui/x-date-pickers/DatePicker';
import { styled } from '@mui/material/styles';
import { Section, MaroonButton } from '../../../components';
import dayjs from 'dayjs';

const FilterTextfield = styled(TextField)`
  border-radius: 1px;
`;

export type ReplaylistFilters = {
  ids?: number[];
  nameContains?: string;
  dateFrom?: string;
  dateTo?: string;
};

type ReplaylistFiltersClusterProps = {
  filters: ReplaylistFilters;
  onFiltersUpdated: (newFilters: ReplaylistFilters, submit: boolean) => void;
};

export const ReplaylistFiltersCluster = ({ filters, onFiltersUpdated }: ReplaylistFiltersClusterProps) => {
  return (
    <Section>
      <Box sx={{ display: "grid", gridTemplateColumns: 'max-content max-content auto 1fr', gap: 2, alignItems: 'center' }}>
        <Typography>Replay Name:</Typography>
        <FilterTextfield
          id="name"
          label="Name"
          size="small"
          fullWidth
          value={filters.nameContains ?? ""}
          onChange={(event: React.ChangeEvent<HTMLInputElement>) => {
            onFiltersUpdated({
                ...filters,
                nameContains: event.target.value
            }, false);
          }}
          onKeyDown={(event) => {
            if (event.key === 'Enter') {
              onFiltersUpdated(filters, true);
            }
          }}
        />
        <MaroonButton onClick={() => onFiltersUpdated(filters, true)}>🔎Search</MaroonButton>
        <Box/>

        <Typography>Date Uploaded:</Typography>
        <Stack direction="row" spacing={1}>
          <DatePicker
            label="From"
            value={filters.dateFrom ? dayjs(filters.dateFrom) : null}
            onChange={(newValue) => {
              onFiltersUpdated({
                ...filters,
                dateFrom: newValue?.format('YYYY-MM-DD')
              }, true);
            }}
            slotProps={{ textField: { size: 'small' } }}
          />
          <DatePicker
            label="To"
            value={filters.dateTo ? dayjs(filters.dateTo) : null}
            onChange={(newValue) => {
              onFiltersUpdated({
                ...filters,
                dateTo: newValue?.format('YYYY-MM-DD')
              }, true);
            }}
            slotProps={{ textField: { size: 'small' } }}
          />
        </Stack>
        <MaroonButton onClick={() => onFiltersUpdated({}, true)}>✖️Clear</MaroonButton>
        <Box/>
      </Box>
    </Section>
  )
};
