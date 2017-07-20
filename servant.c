/* server.c */
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#define LOCAL_SERVER_PORT 13107
#define BUF 255
enum { SAMPLE_COUNT = 1000 };

uint32_t sampleA[SAMPLE_COUNT] = {0};
uint32_t sampleB[SAMPLE_COUNT] = {0};
uint32_t sampleC[SAMPLE_COUNT] = {0};
int main(int argc, char **argv) {
  int s, rc, n, len;
  struct sockaddr_in cliAddr, servAddr;
  uint32_t puffer[100];
  uint32_t reply[100];
  time_t time1;
  char loctime[BUF];
  char *ptr;
  const int y = 1;
  /* Socket erzeugen */
  s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s < 0) {
    printf("%s: Kann Socket nicht öffnen ...(%s)\n", argv[0], strerror(errno));
    exit(EXIT_FAILURE);
  }
  /* Lokalen Server Port bind(en) */
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(LOCAL_SERVER_PORT);
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
  rc = bind(s, (struct sockaddr *)&servAddr, sizeof(servAddr));
  if (rc < 0) {
    printf("%s: Kann Portnummern %d nicht binden (%s)\n", argv[0],
           LOCAL_SERVER_PORT, strerror(errno));
    exit(EXIT_FAILURE);
  }
  printf(
      "%s: Wartet auf Daten am Port (UDP) %u. wating 1s to flush buffer...\n",
      argv[0], LOCAL_SERVER_PORT);
  sleep(1);
  /* Serverschleife */
  memset(puffer, 0, sizeof(puffer));
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    /* Puffer initialisieren */

    /* Nachrichten empfangen */
    len = sizeof(cliAddr);
    n = recvfrom(s, (char *)puffer, BUF, 0, (struct sockaddr *)&cliAddr, &len);
    if (n < 0) {
      printf("%s: Kann keine Daten empfangen ...\n", argv[0]);
      continue;
    }
    rc = sendto(s, (char *)puffer, n, 0, (struct sockaddr *)&cliAddr, len);

    sampleA[i] = puffer[0];
    sampleB[i] = puffer[1];
    sampleC[i] = puffer[2];
    if (0)
      if (sampleA[i] - 1 == sampleC[i]) {
        if (sampleB[i] > 80000) {
          return -42;
        }
      }
  }
  printf("Got %d Samples\n", SAMPLE_COUNT);
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    if (sampleA[i] - 1 == sampleC[i])
      printf("%d, %d, %d\n", sampleA[i], sampleB[i], sampleC[i]);
  }
  return EXIT_SUCCESS;
}
