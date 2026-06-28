const BASE_URL = '/api/v1';

interface RequestOptions extends RequestInit {
  body?: any;
}

async function apiCall<T>(endpoint: string, options: RequestOptions = {}) {
  const url = `${BASE_URL}${endpoint}`;

  const headers = new Headers(options.headers);
  if (options.body && typeof options.body === 'object') {
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

  return response.json() as Promise<T>;
}

export async function apiGET<T>(endpoint: string, options: RequestOptions = {}) {
  return apiCall<T>(endpoint, {
    method: 'GET',
    ...options
  });
}

export async function apiPOST<T>(endpoint: string, options: RequestOptions = {}) {
  return apiCall<T>(endpoint, {
    method: 'POST',
    ...options
  });
}
