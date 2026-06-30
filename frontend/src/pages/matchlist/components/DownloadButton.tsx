import { useState } from 'react';
import { MaroonButton } from '../../../components';
import { apiDownloadFile } from '../../../api/client.ts';

export type DownloadButtonProps = {
  endpoint: string;
}

export const DownloadButton = (props: DownloadButtonProps) => {
  const [loading, setLoading] = useState(false);

  const onClick = async () => {
    try {
      setLoading(true);
      await apiDownloadFile(props.endpoint);
    } catch (err) {
      console.log(err);
    } finally {
      setLoading(false);
    }
  };

  const text = loading
    ? '🔄Loading'
    : '⬇️Download'

  return (<MaroonButton onClick={onClick}>{text}</MaroonButton>);
};
