import React, { useMemo } from 'react'
import * as d3 from 'd3'

const textMax = 120
const svgWidth = 600
const svgHeight = 300
const margin = {
	top: 20,
	right: 20,
	bottom: 100,
	left: 100,
}
const graphWidth = svgWidth - margin.left - margin.right
const graphHeight = svgHeight - margin.top - margin.bottom

export default ({ fortunes = [] }) => {
	const svg = d3
		.select('.canvas')
		.append('svg')
		.attr('width', svgWidth)
		.attr('height', svgHeight)
	const graph = svg
		.append('g')
		.attr('width', graphWidth)
		.attr('height', graphHeight)
		.attr('transform', `translate(${margin.left}, ${margin.top})`)
	const xAxisGroup = graph
		.append('g')
		.attr('transform', `translate(0, ${graphHeight})`)
	const yAxisGroup = graph.append('g')
	//const maxShown = d3.max(fortunes, d => d.shown)
	const maxScore = d3.max(fortunes, d => d.score)
	//const maxVotes = d3.max(fortunes, d => d.votes)

	const yScale = d3
		.scaleLinear()
		.domain([0, maxScore])
		.range([graphHeight, 0])
	const xScale = d3
		.scaleBand()
		.domain(fortunes.map(item => item.text))
		.range([0, graphWidth])
		.paddingInner(0.2)

	const rects = graph.selectAll('rect').data(fortunes)
	rects
		.enter()
		.append('rect')
		.attr('width', xScale.bandwidth)
		.attr('height', d => graphHeight - yScale(d.score))
		.attr('x', d => xScale(d.text.slice(0, textMax)))
		.attr('y', d => yScale(d.score))
		.style('fill', 'white')

	const xAxis = d3.axisBottom(xScale)
	const yAxis = d3
		.axisLeft(yScale)
		.ticks(3)
		.tickFormat(d => `${d} shown`)

	xAxisGroup.call(xAxis)
	yAxisGroup.call(yAxis)

	xAxisGroup
		.selectAll('text')
		.attr('text-anchor', 'end')
		.attr('transform', 'rotate(-40)')
		.attr('fill', 'white')
		.attr('font-size', '.5rem')

	return <div className="canvas"></div>
}
