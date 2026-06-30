import { type Match } from '../../../types/match.ts';
import { Table, TableBody, TableCell, TableHead, TableRow, Box, Stack } from '@mui/material';
import { styled } from '@mui/material/styles';
import { useReactTable, createColumnHelper, getCoreRowModel, flexRender } from '@tanstack/react-table';
import { MaroonButton } from '../../../components';
import { DownloadButton } from './DownloadButton.tsx';

interface MatchlistTableProps {
  data: Match[];
}

const columnHelper = createColumnHelper<Match>();
const columns = [
  columnHelper.accessor('date', {
    header: 'Date',
    cell: (info) => info.getValue()
  }),
  columnHelper.accessor('name', {
    header: 'Name',
    cell: (info) => info.getValue()
  }),
  columnHelper.accessor('duration', {
    header: 'Duration',
    cell: (info) => info.getValue()
  })
];

const MatchlistTableCell = styled(TableCell)`
  border-bottom: 1px solid var(--color-offblack);
`;

export const MatchlistTable = (props: MatchlistTableProps) => {
  const tanTable = useReactTable({
    columns,
    data: props.data,
    getCoreRowModel: getCoreRowModel(),
  });
  return (
    <Box>
      <Table>
        <TableHead>
          {tanTable.getHeaderGroups().map((headerGroup) => (
            <TableRow key={headerGroup.id}>
              {headerGroup.headers.map((header) => (
                <MatchlistTableCell key={header.id}>
                  {header.isPlaceholder ? null : flexRender(header.column.columnDef.header, header.getContext())}
                </MatchlistTableCell>
              ))}
              <MatchlistTableCell>Actions</MatchlistTableCell>
            </TableRow>
          ))}
        </TableHead>
        <TableBody>
          {tanTable.getRowModel().rows.map((row) => (
            <TableRow key={row.id}>
              {row.getVisibleCells().map((cell) => (
                <MatchlistTableCell key={cell.id}>
                  {flexRender(cell.column.columnDef.cell, cell.getContext())}
                </MatchlistTableCell>
              ))}
              <MatchlistTableCell>
                <Stack direction="row" spacing={1}>
                  <MaroonButton>📊Analyze</MaroonButton>
                  <DownloadButton endpoint={`/replay?id=${row.original.id}`} />
                </Stack>
              </MatchlistTableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
    </Box>
  );
};
