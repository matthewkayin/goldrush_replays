import { Section, SectionHeader } from '../../components/SectionHeader.tsx';
import { useState, useCallback, useEffect } from 'react';
import { Container, Table, TableBody, TableCell, TableHead, TableRow, Box, Link, Button } from '@mui/material';
import { styled } from '@mui/material/styles';
import { type Match } from '../../types/match.ts';
import { useReactTable, createColumnHelper, getCoreRowModel, flexRender } from '@tanstack/react-table';
import { UploadDialog } from './components/UploadDialog.tsx';
import { apiGet, apiPost, type ApiResponse } from '../../api/client.ts';

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
  const [uploadDialogOpen, setUploadDialogOpen] = useState(false);
  const [data, setData] = useState<Match[]>([]);

  // GET MATCH DATA
  useEffect(() => {
    // Cleans up pending requests if the component unmounts
    const abortController = new AbortController();

    const fetchData = async () => {
      try {
        const response = await apiGet<ApiResponse<Match>>('/match');
        setData(response.data);
      } catch (err) {
        if (err.name !== 'AbortError') {
          console.log(err.message);
        }
      }
    };

    fetchData();
    return () => abortController.abort();
  }, []);

  // TABLE
  const tanTable = useReactTable({
    columns,
    data,
    getCoreRowModel: getCoreRowModel(),
  });

  // SUBMIT
  const onSubmit = useCallback(async (files: File[]) => {
    const formData = new FormData();
    files.forEach((file: File) => {
      formData.append('files', file);
    });

    try {
      const response = await apiPost<ApiResponse<Match>>('/match', {
        body: formData
      });
      setData(response.data);
    } catch (err) {
      console.log('Error: ', err);
    }
  }, []);

  return (
    <Container>
      <Section>
        <SectionHeader>Replays</SectionHeader>
      </Section>
      <Section>
        <Button onClick={(e) => { e.preventDefault(); setUploadDialogOpen(true); }}>⬆️Upload New Replay</Button>
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
                <MatchlistTableCell><Link href="#">📊Analyze</Link> <Link href="#">⬇️Download</Link></MatchlistTableCell>
              </TableRow>
            ))}
          </TableBody>
        </Table>
      </Box>

      <UploadDialog
      open={uploadDialogOpen}
      onClose={() => setUploadDialogOpen(false)}
      onSubmit={onSubmit}
      />
    </Container>
  );
};
