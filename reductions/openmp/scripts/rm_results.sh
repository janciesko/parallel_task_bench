#!/bin/bash

benchmark1="fib"
benchmark2="powerset"
benchmark3="dot"
benchmark4="powerset-UDR"

rm -f ../../results/$benchmark1/overN/*.res
rm -f ../../results/$benchmark1/overN/PNG/*
rm -f ../../results/$benchmark1/overN/SVG/*
rm -f ../../results/$benchmark1/overCutOff/*.res
rm -f ../../results/$benchmark1/overCutOff/PNG/*
rm -f ../../results/$benchmark1/overCutOff/SVG/*

rm -f ../../results/$benchmark2/overN/*.res
rm -f ../../results/$benchmark2/overN/PNG/*
rm -f ../../results/$benchmark2/overN/SVG/*
rm -f ../../results/$benchmark2/overCutOff/*.res
rm -f ../../results/$benchmark2/overCutOff/PNG/*
rm -f ../../results/$benchmark2/overCutOff/SVG/*

rm -f ../../results/$benchmark3/overN/*.res
rm -f ../../results/$benchmark3/overN/PNG/*
rm -f ../../results/$benchmark3/overN/SVG/*
rm -f ../../results/$benchmark3/overCutOff/*.res
rm -f ../../results/$benchmark3/overCutOff/PNG/*
rm -f ../../results/$benchmark3/overCutOff/SVG/*

rm -f ../../results/$benchmark4/overRT/*.res
rm -rf ../../results/$benchmark4/overRT/PNG/*
rm -rf ../../results/$benchmark4/overRT/SVG/*
