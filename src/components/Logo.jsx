import React from 'react'
import { css } from 'glamor'
import { useSelector } from 'react-redux'

//import chip from '../static/img/chip.svg'
import robot from '../static/img/robot.svg'

export default props => {
	const theme = useSelector(state => state.theme) || []

	return (
		<React.Fragment>
			<img
				src={robot}
				{...css({
					position: 'relative',
					left: '-.35rem',
					opacity: 0.4,
					width: '35%',
					margin: '2rem auto 0 auto',
					display: 'block',
					[`@media(min-width: ${theme.breaks.md}px)`]: {
						margin: '2rem 0 1rem 1.5rem',
						maxWidth: '8rem',
					},
				})}
				alt="Fortune Calibrator"
			/>
			<section
				{...css({
					position: 'relative',
					minHeight: '11rem',
					maxWidth: '80%',
					margin: '1rem auto 0 auto',
					[`@media(min-width: ${theme.breaks.md}px)`]: {
						minHeight: '5rem',
						margin: '0 0 2rem 0',
						left: '1.5rem',
					},
					'& h1': {
						fontSize: '4.25rem',
						fontWeight: 300,
						lineHeight: '4rem',
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
						top: -5,
						left: -5,
						color: 'rgba(0, 0, 0, 0.7)',
					},
					'& h1:nth-child(3)': {
						position: 'absolute',
						top: -10,
						left: -10,
						color: '#fff',
					},
				})}
				{...css(props.css)}>
				<h1>FORTUNE Calibrator</h1>
				<h1>FORTUNE Calibrator</h1>
				<h1>FORTUNE Calibrator</h1>
			</section>
		</React.Fragment>
	)
}
