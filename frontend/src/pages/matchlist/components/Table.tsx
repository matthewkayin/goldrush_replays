import { type Replay } from '../../../types/replay.ts';
import { Table, TableBody, TableCell, TableHead, TableRow, Box, Stack } from '@mui/material';
import { styled } from '@mui/material/styles';
import { useReactTable, createColumnHelper, getCoreRowModel, flexRender } from '@tanstack/react-table';
import { MaroonButton } from '../../../components';
import { DownloadButton } from './DownloadButton.tsx';

interface ReplaylistTableProps {
  data: Replay[];
}

const columnHelper = createColumnHelper<Replay>();
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

const ReplaylistTableCell = styled(TableCell)`
  border-bottom: 1px solid var(--color-offblack);
`;

export const ReplaylistTable = (props: ReplaylistTableProps) => {
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
                <ReplaylistTableCell key={header.id}>
                  {header.isPlaceholder ? null : flexRender(header.column.columnDef.header, header.getContext())}
                </ReplaylistTableCell>
              ))}
              <ReplaylistTableCell>Actions</ReplaylistTableCell>
            </TableRow>
          ))}
        </TableHead>
        <TableBody>
          {tanTable.getRowModel().rows.map((row) => (
            <TableRow key={row.id}>
              {row.getVisibleCells().map((cell) => (
                <ReplaylistTableCell key={cell.id}>
                  {flexRender(cell.column.columnDef.cell, cell.getContext())}
                </ReplaylistTableCell>
              ))}
              <ReplaylistTableCell>
                <Stack direction="row" spacing={1}>
                  <MaroonButton>📊Analyze</MaroonButton>
                  <DownloadButton endpoint={`/replay-data?id=${row.original.id}`} />
                </Stack>
              </ReplaylistTableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
    </Box>
  );
};
