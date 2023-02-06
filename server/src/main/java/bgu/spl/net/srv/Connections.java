package bgu.spl.net.srv;

import java.util.Set;

public interface Connections<T> {

    boolean send(int connectionId, T msg);


    void disconnect(int Id);

    void addSubscription(String channel, int connectionId, int Id);

    void addConnection(int connectionId, ConnectionHandler<T> connectionHandler);

    String removeSubscription(int connectionId, int Id);

    void addUser(String username, String passcode, int ConnectionId);

    boolean isUserLoggedIn(String username);

    boolean isValidLogin(String username, String passcode,int connectionId);

    boolean isUserLoggedIn(int connectionId);

    boolean userExists(String username);

    boolean isSubscribed(int connectionId, String destination);

    Integer getSubscriptionId(int connectionId, String destination);

    Set<Integer> getSubscriptionsByChannelName(String channel);
}
