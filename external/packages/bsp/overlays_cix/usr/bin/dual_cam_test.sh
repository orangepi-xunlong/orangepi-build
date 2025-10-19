#gst-launch-1.0 v4l2src device=/dev/video1 ! videoconvert ! autovideosink &
gst-launch-1.0 v4l2src device=/dev/video1 ! videoconvert ! autovideosink \
			   v4l2src device=/dev/video3 ! videoconvert ! autovideosink

#gst-launch-1.0 v4l2src device=/dev/video3 ! videoconvert ! autovideosink &

