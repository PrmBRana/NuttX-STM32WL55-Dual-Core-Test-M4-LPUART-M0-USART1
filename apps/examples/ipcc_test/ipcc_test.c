/****************************************************************************
 * apps/examples/ipcc_test/ipcc_test.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int ipcc_test_main(int argc, char *argv[])
{
  const char *msg = "Hello from M4!";
  char rxbuf[32];
  ssize_t nbytes;
  int fd;

  printf("IPCC Test starting...\n");

  /* Open IPCC channel 1 — chan index 0 = /dev/ipcc0 */

  fd = open("/dev/ipcc0", O_RDWR);
  if (fd < 0)
    {
      printf("ERROR: open /dev/ipcc0 failed, errno=%d\n", errno);
      return -1;
    }

  printf("Opened /dev/ipcc0 OK\n");

  /* Write dummy data to M0+ */

  nbytes = write(fd, msg, strlen(msg));
  if (nbytes < 0)
    {
      printf("ERROR: write failed, errno=%d\n", errno);
      close(fd);
      return -1;
    }

  printf("Sent %zd bytes: \"%s\"\n", nbytes, msg);

  /* Read reply from M0+ */

  printf("Waiting for M0+ reply...\n");
  memset(rxbuf, 0, sizeof(rxbuf));

  nbytes = read(fd, rxbuf, sizeof(rxbuf) - 1);
  if (nbytes < 0)
    {
      printf("ERROR: read failed, errno=%d\n", errno);
      close(fd);
      return -1;
    }

  rxbuf[nbytes] = '\0';
  printf("Received %zd bytes: \"%s\"\n", nbytes, rxbuf);

  close(fd);
  printf("IPCC Test done.\n");
  return 0;
}