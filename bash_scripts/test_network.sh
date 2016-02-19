#!/bin/bash

# The following 2 arrays list the ip address of different respectively
#
listA="52.90.241.168 52.91.199.130 52.87.250.5"
listB="54.173.210.29 54.152.8.117 54.86.173.53"

function install_netperf
{
	for server in $(echo "$1 $2");do
		ssh -i cityu_cloud_computing_key.pem ubuntu@$server "sudo apt-get update && sudo apt-get install netperf -y"
	done
}

function install_iperf
{
	for server in $(echo "$1 $2");do
                ssh -i cityu_cloud_computing_key.pem ubuntu@$server "sudo apt-get update && sudo apt-get install iperf -y"
        done
}

function kill_netserver
{
	# $1 - server ip address
	# $2 - server internal ip address
	ssh -i cityu_cloud_computing_key.pem ubuntu@$1 "sudo pkill netserver" && sleep 5
	ssh -i cityu_cloud_computing_key.pem ubuntu@$1 "sudo netserver -L $2"
}

function kill_iserver
{
	# $1 - server ip address
	# $2 - server internal ip address
	ssh -i cityu_cloud_computing_key.pem ubuntu@$server "sudo pkill iperf" && sleep 5
	ssh -i cityu_cloud_computing_key.pem ubuntu@$server "sudo iperf -s -w 256K"
}

function netperf_start
{
	# $1 - client ip address
	# $2 - target server ip address
	ssh -i cityu_cloud_computing_key.pem ubuntu@$1 "sudo netperf -H $2"
}

function iperf_start
{
	# $1 - client ip address
	# $2 - target server ip address
	ssh -i cityu_cloud_computing_key.pem ubuntu@$1 "sudo iperf -c $2 -w 256K"
}

function speed_test
{
	# $1 - servers in list A
	# $2 - servers in list B
	# $3 - operations
	for server in $1;do
		server_internal_ip="$(ssh -i cityu_cloud_computing_key.pem ubuntu@$server "sudo ip addr show eth0 | egrep 'inet ' | xargs | cut -d ' ' -f 2 | cut -d '/' -f 1")"
		[ "$3" = "net" ] && kill_netserver "$server" "$server_internal_ip" || kill_iserver "$server" "$server_internal_ip"

		for client in $2;do
			client_internal_ip="$(ssh -i cityu_cloud_computing_key.pem ubuntu@$client "sudo ip addr show eth0 | egrep 'inet ' | xargs | cut -d ' ' -f 2 | cut -d '/' -f 1")"
			echo -e "\nTesting connection speed:\nServer: $server ($server_internal_ip)\nClient: $client ($client_internal_ip)"
			[ "$3" = "net" ] && netperf_start "$server" "$server_internal_ip" || iperf_start "$server" "$server_internal_ip"
		done
	
	done

}
function print_usage
{
	echo -e "\nUsage: $0 [nstA2B|istA2B|nstB2A|istB2A|in|ii|pA|pB]\n"
        echo -e "\tnstA2B - test network speed with netperf (from group A to group B)"
        echo -e "\tistA2B - test network speed with iperf (from group A to group B)"
        echo -e "\tnstB2A - test network speed with netperf (from group B to group A)"
        echo -e "\tistB2A - test network speed with iperf (from group B to group A)"
        echo -e "\tin - install netperf package"
        echo -e "\tii - install iperf package"
	echo -e "\tpA - print servers list A ip address"
	echo -e "\tpB - print servers list B ip address\n"
}

if [ $# != 1 ];then
	print_usage
	exit
fi

case "$1" in

	"nstA2B") speed_test "$listA" "$listB" "net" ;;
	"istA2B") speed_test "$listA" "$listB" "iperf" ;;
	"nstB2A") speed_test "$listB" "$listA" "net" ;;
	"istB2A") speed_test "$listB" "$listA" "iperf" ;;
	"in") install_netperf "$listA" "$listB" ;;
	"ii") install_iperf "$listA" "$listB" ;;
	"pA") echo $listA ;;
	"pB") echo $listB ;;
	*) print_usage ;;

esac
