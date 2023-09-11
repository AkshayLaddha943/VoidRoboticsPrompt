#include "rclcpp/rclcpp.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

using namespace std::chrono_literals;
using NavigateToPose = nav2_msgs::action::NavigateToPose;

class NavigationNode : public rclcpp::Node
{
public:
    NavigationNode()
        : Node("navigation_node")
    {
        // Create an action client for NavigateToPose
        this->client_ = rclcpp_action::create_client<nav2_msgs::action::NavigateToPose>(this, "/navigate_to_pose");
        while (!client_->wait_for_action_server(10s))
        {
            if (!rclcpp::ok())
            {
                RCLCPP_ERROR(get_logger(), "Interrupted while waiting for action server.");
                return;
            }
            RCLCPP_INFO(get_logger(), "Action server not available. Retrying...");
        }
        RCLCPP_INFO(get_logger(), "Action server is available.");
    }

    void cancelGoals()
    {
        if (!client_->wait_for_action_server(10s))
        {
            RCLCPP_ERROR(get_logger(), "Action server not available");
            return;
        }

        /*auto send_goal_options = rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::CancelCallback();
        auto goal_handle = rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::GoalHandle();
        rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::SharedPtr goal_handle;
        typename rclcpp_action::CancelResponse<nav2_msgs::action::NavigateToPose>::SharedPtr send_goal_options;*/

        // Wait for the result of the cancel action
        if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), this->client_->async_cancel_all_goals()) !=
            rclcpp::FutureReturnCode::SUCCESS)
        {
            RCLCPP_ERROR(get_logger(), "Failed to send cancel request.");
        }
        else
        {
            RCLCPP_INFO(get_logger(), "Cancel request sent successfully.");
        }
    }

private:
    rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SharedPtr client_;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<NavigationNode>();

    // Call the cancelGoals function whenever you want to cancel the goal
    node->cancelGoals();

    rclcpp::shutdown();
    return 0;
}
