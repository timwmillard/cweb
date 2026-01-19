
#include "ecewo.h"

#include "cweb.h"

void health(Req *req, Res *res) {
  send_json(res, OK, "{\"version\": \"" VERSION "\"}\n");
}
