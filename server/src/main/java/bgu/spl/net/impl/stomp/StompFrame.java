package bgu.spl.net.impl.stomp;

import java.util.Map;

public class StompFrame {
    private String command;
    private Map<String, String> headers;
    private String body;

    public StompFrame(String command, Map<String, String> headers, String body) {
        this.command = command;
        this.headers = headers;
        this.body = body;
    }

    public String getCommand() {
        return command;
    }

    public Map<String, String> getHeaders() {
        return headers;
    }

    public String getBody() {
        return body;
    }


    @Override
    public String toString() {
        StringBuilder result = new StringBuilder(command + "\n");
        for (Map.Entry<String, String> header : headers.entrySet()) {
            result.append(header.getKey()).append(":").append(header.getValue()).append("\n");
        }
        // result.append("\n").append(body).append("\u0000");
        result.append("\n").append(body);
        return result.toString();
    }
}
