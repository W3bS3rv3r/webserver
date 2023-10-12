#!/bin/python3

import argparse
import concurrent.futures
import requests

# Function to send a request and print the response
def send_request(request_num, url):
    response = requests.get(url)
    print(f'Request {request_num} - Status code: {response.status_code}')

if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='Send multiple requests to a server concurrently.')
    parser.add_argument('url', type=str, help='Target URL to send requests to')
    parser.add_argument('num_requests', type=int, help='Number of concurrent requests to send')
    args = parser.parse_args()

    url = "http://" + args.url
    num_requests = args.num_requests

    # Create a ThreadPoolExecutor with the desired number of threads
    with concurrent.futures.ThreadPoolExecutor(max_workers=num_requests) as executor:
        # Submit tasks to the executor
        # Each task will send a request
        future_to_request = {executor.submit(send_request, i, url): i for i in range(1, num_requests + 1)}

        # Wait for all tasks to complete
        concurrent.futures.wait(future_to_request)

        # Print any exceptions that occurred during the tasks
        for future in concurrent.futures.as_completed(future_to_request):
            try:
                future.result()
            except Exception as e:
                print(f'An error occurred: {str(e)}')
