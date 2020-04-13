import React from 'react'
import { css } from 'glamor'
import { useSelector } from 'react-redux'

export default props => {
	const theme = useSelector(state => state.theme) || []

	return (
		<section
			{...css({
				position: 'relative',
				minHeight: '10rem',
				margin: '6rem 2rem 0 2rem',
				[`@media(min-width: ${theme.breaks.md}px)`]: {
					minHeight: '5rem',
					margin: '1.75rem',
				},
				'& h1': {
					fontSize: '4.5rem',
					fontWeight: 300,
					margin: 0,
					textAlign: 'center',
					color: 'rgba(0, 0, 0, 0.4)',
					[`@media(min-width: ${theme.breaks.md}px)`]: {
						fontSize: '3.5rem',
					},
				},
				'& h1:nth-child(1)': {
					position: 'absolute',
					top: 0,
					left: 0,
				},
				'& h1:nth-child(2)': {
					position: 'absolute',
					top: '-5px',
					left: '-5px',
					color: 'rgba(0, 0, 0, 0.7)',
				},
				'& h1:nth-child(3)': {
					position: 'absolute',
					top: '-10px',
					left: '-10px',
					color: '#fff',
				},
			})}
			{...css(props.css)}>
			<h1>FORTUNE Calibrator</h1>
			<h1>FORTUNE Calibrator</h1>
			<h1>FORTUNE Calibrator</h1>
		</section>
	)
}
