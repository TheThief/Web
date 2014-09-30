#include "responses.h"

#include "HTTPResponse.h"

char status301content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>301 Permenantly Moved</h1>\r\n"
	"<p>You probably missed a / off of a folder url. See the \"Location\" header for details.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status301(301, "Permenantly Moved", sizeof(status301content)-1, status301content);

char status400content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>400 Bad Request</h1>\r\n"
	"<p>This is a <i>HTTP</i> server. Please send a valid request.<!--Idiot--></p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status400(400, "Bad Request", sizeof(status400content)-1, status400content);

char status400nohost_content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>400 Bad Request (no host header)</h1>\r\n"
	"<p>HTTP/1.1 requires you to send a \"Host:\" header with your request. Please do.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status400nohost(400, "Bad Request (no host header)", sizeof(status400nohost_content)-1, status400nohost_content);

char status400badhost_content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>400 Bad Request (bad host header)</h1>\r\n"
	"<p>This server does not serve on the domain you specified in the host header.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status400badhost(400, "Bad Request (bad host header)", sizeof(status400badhost_content)-1, status400badhost_content);

char status403content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>403 Forbidden</h1>\r\n"
	"<p>You're not allowed to access this...</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status403(403, "Forbidden", sizeof(status403content)-1, status403content);

char status404content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>404 File Not Found</h1>\r\n"
	"<p>Maybe if you tried for a file that actually existed you might have more luck :)</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status404(404, "File Not Found", sizeof(status404content)-1, status404content);

char status413content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>413 Request Too Large</h1>\r\n"
	"<p>Please reduce the size of your request.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status413(413, "Request Too Large", sizeof(status413content)-1, status413content);

char status414content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>414 Request URI Too Long</h1>\r\n"
	"<p>Please check your request URI.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status414(414, "Request URI Too Long", sizeof(status414content)-1, status414content);

char status431content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>431 Request Header Fields Too Large</h1>\r\n"
	"<p>Please reduce the size of your request.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status431(431, "Request Header Fields Too Large", sizeof(status431content)-1, status431content);

const char status500content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>500 Internal Server Error</h1>\r\n"
	"<p>Unknown error, probably an internal limitation.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status500(500, "Internal Server Error", sizeof(status500content)-1, status500content);

const char status501content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>501 Not Implemented</h1>\r\n"
	"<p>This web server only supports GET and HEAD requests.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status501(501, "Not Implemented", sizeof(status501content)-1, status501content);

const char status505content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>505 HTTP Version Not Supported</h1>\r\n"
	"<p>This web server only supports HTTP/1.1 requests.</p>\r\n"
	"<p>If you are still using HTTP/1.0, Get with the times already.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status505(505, "HTTP Version Not Supported", sizeof(status505content)-1, status505content);
