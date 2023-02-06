package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.atomic.AtomicInteger;

public class StompProtocol implements StompMessagingProtocol<String> {

    private int connectionId;
    private Connections<String> connections;
    private boolean shouldTerminate = false;


    /**
     * headers constants
     */
    private final String ACCEPT_VERSION = "accept-version";
    private final String HOST_NAME = "stomp.cs.bgu.ac.il";
    private final String HOST_HEADER = "host";
    private final String LOGIN = "login";
    private final String PASSCODE = "passcode";
    private final String RECEIPT = "receipt";
    private final String ID = "id";
    private final String DESTINATION = "destination";


    private static AtomicInteger messageId = new AtomicInteger(0);


    @Override
    public void start(int connectionId, Connections connections) {
        this.connectionId = connectionId;
        this.connections = connections;
    }


    @Override
    public String process(String message) {
        StompFrame frame = parseMessage(message);
        String returnFrame = handleFrame(frame);
        return returnFrame;
    }


    /**
     * this parses the message to a STOMP frame
     *
     * @param message
     * @return
     */
    private StompFrame parseMessage(String message) {
        String[] lines = message.split("\\n");
        String command = lines[0];
        Map<String, String> headers = new HashMap<>(); // <Key>:<Value>
        int i = 1;
        while (i < lines.length && !lines[i].isEmpty()) {
            String[] header = lines[i].split(":", 2);
            String headerString = header[0].replaceAll("\\s", "");
            String headerValue = header[1].replaceAll("\\s", "");
            if(headerValue.charAt(0) == '/'){ // in case of a game
                headerValue = headerValue.substring(1);
            }
            headers.put(headerString, headerValue);
            i++;
        }
        // The body is the remaining lines after the headers, joined by newline characters
        String body = String.join("\n", Arrays.copyOfRange(lines, i, lines.length));

        return new StompFrame(command, headers, body.toString());
    }

    /**
     * this method handles the frame according to the command sent from the client
     *
     * @param frame
     */
    private String handleFrame(StompFrame frame) {
        switch (frame.getCommand()) {
            case "CONNECT":
                return handleConnect(frame);
            case "SEND":
                return handleSend(frame);
            case "SUBSCRIBE":
                return handleSubscribe(frame);
            case "UNSUBSCRIBE":
                return handleUnsubscribe(frame);
            case "DISCONNECT":
                return handleDisconnect(frame);
            default:
                String answer = sendErrorFrame("Not a valid command");
                return answer;
        }
    }

    /**
     * if the client sent a 'CONNECT' request
     *
     * @param frame
     */
    private String handleConnect(StompFrame frame) {
        // First, check if the required headers (login and passcode) are present
        Map<String, String> headers = frame.getHeaders();
        String acceptVersion = headers.get(ACCEPT_VERSION);
        String host = headers.get(HOST_HEADER);
        String login = headers.get(LOGIN);
        String passcode = headers.get(PASSCODE);
        String receipt = headers.get(RECEIPT);

        // check if accept-version is 1.2 and host is stomp.cs.bgu.ac.il
        if (acceptVersion != null && acceptVersion.equals("1.2")
                && host != null && host.equals(HOST_NAME)) {
            // check if login and passcode are present
            if (login != null && !login.isEmpty() && passcode != null && !passcode.isEmpty()) {

                if (!connections.userExists(login)) {
                    // in case of a new user
                    // adds the user to the list and logs him in
                    connections.addUser(login, passcode, connectionId);
                    // sends a CONNECTED frame back to the client
                    System.out.println("user connected");
                    return sendConnectedFrame(receipt);
                }

                // check if user is already logged in
                if (connections.isUserLoggedIn(login)) {
                    // send STOMP error frame indicating user is already logged in
                    return sendErrorFrame("User already logged in");
                }
                // check if user exists and password matches
                if (connections.isValidLogin(login, passcode, connectionId)) {
                    System.out.println("Login successful");
                    // send CONNECTED frame
                    return sendConnectedFrame(receipt);
                    // print "Login successful"

                } else {
                    // send STOMP error frame indicating wrong password
                    return sendErrorFrame("Wrong password");
                }
            }
        } else {
            // send STOMP error frame indicating unsupported version or invalid host
            return sendErrorFrame("Unsupported version or invalid host");
        }
        return null;
    }


    /**
     * handles a  'SEND' request
     *
     * @param frame
     */
    private String handleSend(StompFrame frame) {
        Map<String, String> headers = frame.getHeaders();
        String destination = headers.get(DESTINATION); //removes the '/'
        String receipt = headers.get(RECEIPT);
        String body = frame.getBody();

        // check if the client is subscribed to the destination topic
        if (connections.isSubscribed(connectionId, destination)) {
            if (destination == null) {
                return sendErrorFrame("Destination can't be null");
            }


            //Send a MESSAGE frame back to the client
            Integer subscriptionId = connections.getSubscriptionId(connectionId, destination);
            String sub = subscriptionId.toString();
            return sendMessageFrame(sub, destination, body, receipt);
        } else {
            // send ERROR frame
            return sendErrorFrame("You're not subscribed to this channel, thus you're not allowed to send messages to it");
        }
    }


    /**
     * if the client sent a 'SUBSCRIBE' request
     *
     * @param frame
     */
    private String handleSubscribe(StompFrame frame) {
        Map<String, String> headers = frame.getHeaders();
        String destination = headers.get(DESTINATION);
        String id = headers.get(ID);
        String receipt = headers.get(RECEIPT);


        if (tryParse(id) == null) {
            // disconnects the client after he failed to subscribe
            connections.disconnect(connectionId);
            shouldTerminate = true;
            //sends ERROR Message to client
            return sendErrorFrame("Cannot subscribe to topic with invalid id");
        }

        if (!connections.isUserLoggedIn(connectionId)) {
            return sendErrorFrame("cannot subscribe to channel without being logged-in");
        }

        //if the user is Already a part of the channel send error
        if(!connections.isSubscribed(connectionId,destination)){
            // Adds the ID the client sent to the subscriptions list
            connections.addSubscription(destination, connectionId, Integer.parseInt(id));
            System.out.println("Joined channel " + destination);
            // in case a receipt was sent
            if (receipt != null) {
                return sendReceiptFrame(receipt);
            }
        }
        return sendErrorFrame("User already subscribed to the channel");
    }


    /**
     * if the client sent 'UNSUBSCRIBE' request
     *
     * @param frame
     */
    private String handleUnsubscribe(StompFrame frame) {
        String id = frame.getHeaders().get(ID);
        String receipt = frame.getHeaders().get(RECEIPT);

        //Client didn't send an id
        if (id == null) {
            return sendErrorFrame("cannot unsubscribe without an ID");
        }

        // Client send an invalid ID
        if (tryParse(id) == null) {
            return sendErrorFrame("ID should contain only numerical Integer values");
        }

        if (receipt == null || receipt.trim() == "") {
            return sendErrorFrame("Cannot Unsubscribe without a receipt");
        }


        int channelId = Integer.parseInt(id);
        // Remove the subscription for the specified connection
        String GameName = connections.removeSubscription(connectionId, channelId);
        if (GameName == null) {
            return sendErrorFrame("User can't unsubscribe from a channel he's not registered to");
        }
        System.out.println("Exited channel " + GameName);
        return sendReceiptFrame(receipt);
    }


    /**
     * @param frame
     */
    private String handleDisconnect(StompFrame frame) {
        Map<String, String> headers = frame.getHeaders();
        String receipt = headers.get(RECEIPT);

        if (receipt == null || receipt.trim() == "") {
            return sendErrorFrame("Cannot Disconnect without a receipt id");
        }

        if (!connections.isUserLoggedIn(connectionId)) {
            return sendErrorFrame("Before Disconnecting, you might wanna log-in first");
        }

        connections.disconnect(connectionId);
        String returnValue = sendReceiptFrame(receipt);
        System.out.println("user disconnected");
        shouldTerminate = true;
        return returnValue;
    }


    /**
     * @return whether connection should be terminated
     */
    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }


    public Integer tryParse(String value) {
        try {
            return Integer.parseInt(value);
        } catch (NumberFormatException e) {
            return null;
        }
    }


    /**
     * sends an ERROR frame back to the client
     *
     * @param errorMessage
     */
    private String sendErrorFrame(String errorMessage) {
        Map<String, String> headers = new HashMap<>();
        headers.put("message", errorMessage);
        StompFrame errorFrame = new StompFrame("ERROR", headers, "");
        connections.send(connectionId, errorFrame.toString());
        return null;
    }

    /**
     * sends a RECEIPT FRAME back to the client
     *
     * @param receiptId
     */
    private String sendReceiptFrame(String receiptId) {
        Map<String, String> headers = new HashMap<>();
        headers.put("receipt-id", receiptId);
        StompFrame receiptFrame = new StompFrame("RECEIPT", headers, "");
        connections.send(connectionId, receiptFrame.toString());
        // return receiptFrame.toString();
        return null;
    }

    /**
     * sends a CONNECTED frame back to the client
     */
    private String sendConnectedFrame(String receiptId) {
        Map<String, String> connectHeaders = new HashMap<>();
        connectHeaders.put("version", "1.2");
        if (receiptId != null) {
            connectHeaders.put("receipt-id", receiptId);
        }
        StompFrame connectFrame = new StompFrame("CONNECTED", connectHeaders, "");
        String toSend = connectFrame.toString();
        connections.send(connectionId, toSend);
    //    return connectFrame.toString();
        return null;
    }

    /**
     * sends a MESSAGE frame back to the client
     *
     * @param subscription
     * @param destination
     * @param body
     */
    private String sendMessageFrame(String subscription, String destination, String body, String receiptId) {
        Set<Integer> currentChannel = connections.getSubscriptionsByChannelName(destination);
        int msgId = 0;
        if (currentChannel != null) {
            for (Integer connectionId : currentChannel) {
                Map<String, String> headers = new HashMap<>();
                headers.put("subscription", subscription);
                headers.put("message-id", String.valueOf(msgId));
                headers.put("destination", destination);
                if (receiptId != null) {
                    headers.put("receipt-id", receiptId);
                }
                StompFrame messageFrame = new StompFrame("MESSAGE", headers, body);
                System.out.println("sending message:\n" + messageFrame.toString());
                System.out.println(messageFrame.toString());
                connections.send(connectionId, messageFrame.toString());
                msgId++;
            }
        }

        return null;
    }
}
