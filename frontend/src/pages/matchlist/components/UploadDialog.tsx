import { useCallback, useState } from 'react';
import { Modal, Button, Typography, Paper, Stack } from '@mui/material';
import { styled } from '@mui/material/styles';
import CloudUploadIcon from '@mui/icons-material/CloudUpload';
import { useDropzone } from 'react-dropzone';

const ModalBox = styled(Paper)`
  position: absolute;
  top: 40%;
  left: 50%;
  transform: translate(-50%, -50%);
  border: 1px solid var(--color-offblack);
  background-color: var(--color-maroon);
  box-shadow: 24;
`;

const ModalContent = styled(Stack)`
  margin: 16px;
  margin-top: 8px;
`;

const ModalTitle = styled(Typography)`
  font-family: 'Chango', serif;
  color: var(--color-yellow);
  letter-spacing: normal;
  font-size: 16px;
  text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.7);
`;

const DropzoneBox = styled(Stack)`
  width: 400px;
  height: 200px;
  border: 2px dashed var(--color-offblack);
  align-items: center;
  justify-content: center;
  background-color: rgba(0, 0, 0, 0.3);
`;

const ModalButton = styled(Button)`
  background-color: var(--color-tan);
  border: 1px solid var(--color-offblack);
  border-radius: 1px;
  color: var(--color-offlblack);
`;

const AcceptedFilesTypography = styled(Typography)`
  color: #f0f0f0;
  text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.5);
`;

export type UploadFile = {
  lastModified: number;
  name: string;
  path: string;
  relativePath: string;
  size: number;
  type: string;
  webkitRelativePath: string;
}

export type UploadDialogProps = {
  open: boolean;
  onClose: () => void;
  onSubmit?: (files: File[]) => void;
}

export const UploadDialog = ({ open, onClose, onSubmit }: UploadDialogProps) => {
  const [replayFiles, setReplayFiles] = useState<File[]>([]);

  const onDrop = useCallback((acceptedFiles: File[]) => {
    setReplayFiles((prev) => [...prev, ...acceptedFiles]);
  }, []);

  const onCloseCallback = useCallback(() => {
    onClose();
  }, [onClose]);
  const onSubmitCallback = useCallback(() => {
    onClose();
    if (onSubmit && replayFiles) {
      onSubmit(replayFiles);
    }
  }, [onClose, onSubmit, replayFiles]);

  const { getRootProps, getInputProps, isDragActive } = useDropzone({
    accept: {
      'application/octet-stream': ['.rep']
    },
    onDrop
  });

  const dropzoneMessage = isDragActive
    ? 'Drop the files here!'
    : 'Drag and drop a file here or click';
  console.log('rerender ', replayFiles);

  return (
    <Modal
      open={open}
      onClose={onCloseCallback}
    >
      <ModalBox>
        <ModalContent spacing={2}>
          <ModalTitle>Upload Replay</ModalTitle>
          <DropzoneBox
            {...getRootProps()}
            spacing={2}
            sx={{
              ':hover': {
                cursor: 'pointer',
                backgroundColor: 'rgba(0, 0, 0, 0.35)',
                transition: 'background-color 0.3s ease'
              },
              transition: 'background-color 0.3s ease'
            }}
          >
            <input {...getInputProps()} />
            <AcceptedFilesTypography>{dropzoneMessage}</AcceptedFilesTypography>
            <CloudUploadIcon sx={{ fontSize: 40, color: '#f0f0f0' }} />
          </DropzoneBox>
          {replayFiles.length !== 0 &&
          <Stack spacing={0}>
            <AcceptedFilesTypography>Accepted Files:</AcceptedFilesTypography>
              {replayFiles.map((file) => (
                <AcceptedFilesTypography key={file.name}>{file.name}</AcceptedFilesTypography>
              ))}
          </Stack>}
          <Stack direction="row" sx={{ justifyContent: "flex-end" }} spacing={2}>
            <ModalButton onClick={onCloseCallback} sx={{ ':hover': { backgroundColor: '#bf8870' } }}>Cancel</ModalButton>
            <ModalButton
              onClick={onSubmitCallback}
              sx={{ ':hover': { backgroundColor: '#bf8870' } }}>Submit</ModalButton>
          </Stack>
        </ModalContent>
      </ModalBox>
    </Modal>
  );
};
