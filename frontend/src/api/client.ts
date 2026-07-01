const BASE_URL = '/api/v1';

interface RequestOptions extends RequestInit {
  body?: any;
}

export type ApiResponse<T> = {
  data: T[];
  remaining?: number;
}

async function apiCall(endpoint: string, options: RequestOptions = {}): Promise<Response> {
  const url = `${BASE_URL}${endpoint}`;

  const headers = new Headers(options.headers);
  if (options.body && typeof options.body === 'object' && !(options.body instanceof FormData)) {
    headers.set('Content-Type', 'application/json');
    options.body = JSON.stringify(options.body);
  }

  const config: RequestInit = {
    ...options,
    headers
  };
  const response = await fetch(url, config);

  if (!response.ok) {
    const errorData = await response.json().catch(() => { });
    throw new Error(errorData.message || `API call error. Status: ${response.status}.`);
  }

  return response;
}

export async function apiGet<T>(endpoint: string, options: RequestOptions = {}) {
  const response = await apiCall(endpoint, {
    method: 'GET',
    ...options
  });
  return await response.json() as ApiResponse<T>
}

export async function apiPost<T>(endpoint: string, options: RequestOptions = {}) {
  const response = await apiCall(endpoint, {
    method: 'POST',
    ...options
  });
  return await response.json() as ApiResponse<T>
}

export async function apiDownloadFile(endpoint: string, options: RequestOptions = {}) {
  const response = await apiCall(endpoint, {
    method: 'GET',
    ...options
  });

  // Get downloaded filename
  const disposition = response.headers.get('Content-Disposition');
  let filename = 'replay.rep';
  if (disposition) {
    // Regex to handle both standard and UTF-8 encoded filename
    const utf8Match = disposition.match(/filename\*=UTF-8''([^;]+)/i);
    const asciiMatch = disposition.match(/filename="?([^";]+)"?/i);

    if (utf8Match && utf8Match[1]) {
      filename = decodeURIComponent(utf8Match[1]);
    } else if (asciiMatch && asciiMatch[1]) {
      filename = asciiMatch[1];
    }
  }
  console.log('Replay filename: ', filename);

  // Get response blob
  const blob = await response.blob();

  // Create link
  const url = window.URL.createObjectURL(blob);
  const link = document.createElement('a');
  link.href = url;
  link.setAttribute('download', filename);
  document.body.appendChild(link);

  // Click link
  link.click();

  // Cleanup
  link.parentNode.removeChild(link);
  window.URL.revokeObjectURL(url);
}
