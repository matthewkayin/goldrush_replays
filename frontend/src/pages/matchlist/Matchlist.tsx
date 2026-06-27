import { Section, SectionHeader } from '../../components/SectionHeader.tsx';
import { useState } from 'react';
import { Container, Table, TableBody, TableCell, TableHead, TableRow, Box } from '@mui/material';
import { styled } from '@mui/material/styles';
import { type Match } from '../../types/match.ts';
import { useReactTable, createColumnHelper, getCoreRowModel, flexRender } from '@tanstack/react-table';

const defaultData: Match[] = [
  { id: '1', date: '2026-04-01', name: 'Hello friend', duration: '15:22' },
  { id: '2', date: '2026-04-01', name: 'Hello friend', duration: '15:22' },
  { id: '3', date: '2026-04-01', name: 'Hello friend', duration: '15:22' },
  { id: '4', date: '2026-04-01', name: 'Hello friend', duration: '15:22' },
];

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

export const Matchlist = () => {
  const [data, _setData] = useState(() => [...defaultData]);
  const tanTable = useReactTable({
    columns,
    data,
    getCoreRowModel: getCoreRowModel(),
  });

  return (
    <Container>
      <Section>
        <SectionHeader>Replay List</SectionHeader>
      </Section>
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
                <MatchlistTableCell><a href="#">📊Analyze</a> <a href="#">⬇️Download</a></MatchlistTableCell>
              </TableRow>
            ))}
          </TableBody>
        </Table>
      </Box>
    </Container>
  );
};
